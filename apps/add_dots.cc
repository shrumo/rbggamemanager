//
// Created by shrum on 22/07/19.
//

#include <game_state/construction/game_state_creator.h>
#include <stl_extension/argparse.h>
#include <utility/calculate_perft.h>
#include <utility/mark_redundant_dots.h>
#include <utility/printer.h>
#include <utility/shift_table_checker.h>

#include <cassert>
#include <chrono>
#include <fstream>
#include <random>

using namespace rbg;
using namespace std;

const char *kIndentChars = "  ";

std::string times(const std::string &x, size_t n) {
  std::string result;
  for (size_t i = 0; i < n; i++) {
    result += x;
  }
  return result;
}

std::string trim(std::string s) {
  s.erase(s.find_last_not_of(" \n\r\t") + 1);
  s.erase(0, s.find_first_not_of(" \n\r\t"));
  return s;
}

enum class DotsInShifttables { kWhatever, kExclusively, kExclude };

DotsInShifttables dotsInShiftTablesOptionFromString(const std::string &text) {
  if (text == "whatever") {
    return DotsInShifttables::kWhatever;
  }
  if (text == "exclusively") {
    return DotsInShifttables::kExclusively;
  }
  if (text == "exclude") {
    return DotsInShifttables::kExclude;
  }
}

struct PrinterOptions {
  bool modifier_indices = false; // print the indices modifiers in a neat
                                 // comment (ignored in conditionals)
  bool add_dots_in_alternatives = false; // add dots between each alternative
                                         // element (ignored in conditionals)
  DotsInShifttables dots_in_shifttables =
      DotsInShifttables::kWhatever; // disables adding dots in shifttables
  bool add_dots_in_stars =
      false; // add dots inside star statements (ignored in conditionals)
  bool add_dots_after_alternatives =
      false; // add dots after alternative (ignored in conditionals)
  bool add_dots_after_stars =
      false; // add dots after stars (ignored in conditionals)
};

class Printer : public AstFunction<std::string> {
public:
  Printer(
      const PrinterOptions &options,
      const unordered_set<const rbg_parser::game_move *> &moves_to_erase_ = {},
      int indent = 0)
      : options_(options), moves_to_erase_(moves_to_erase_), indent_(indent) {}
  std::string SumCase(const rbg_parser::sum &move) override {
    if (move.get_content().size() == 1) {
      return Printer(options_, moves_to_erase_,
                     indent_)(*move.get_content().front());
    }

    std::string result;

    result += "(";
    result += "\n" + times(kIndentChars, indent_ + 1);

    bool first = true;
    bool is_part_of_shift_table = ContainsOnlyShifts(move);
    for (const auto &m : move.get_content()) {
      if (first) {
        first = false;
      } else {
        result += "+";
        result += "\n" + times(kIndentChars, indent_ + 1);
      }

      if (options_.add_dots_in_alternatives &&
          (options_.dots_in_shifttables != DotsInShifttables::kExclude ||
           !is_part_of_shift_table) &&
          (options_.dots_in_shifttables != DotsInShifttables::kExclusively ||
           is_part_of_shift_table)) {
        result += " .";
      }

      result += Printer(options_, moves_to_erase_, indent_ + 1)(*m);

      if (AddDotAfterSegment(*m) &&
          (options_.dots_in_shifttables != DotsInShifttables::kExclude ||
           !is_part_of_shift_table) &&
          (options_.dots_in_shifttables != DotsInShifttables::kExclusively ||
           is_part_of_shift_table)) {
        result += ".";
      }
    }

    result += "\n" + times(kIndentChars, indent_);
    result += ")";

    return result;
  }

  std::string
  ConcatenationCase(const rbg_parser::concatenation &move) override {
    if (move.get_content().size() == 1) {
      return Printer(options_, moves_to_erase_,
                     indent_)(*move.get_content().front());
    }

    std::string result;

    result += "( ";

    bool first = true;

    for (size_t i = 0; i < move.get_content().size(); i++) {
      const auto &m = move.get_content()[i];
      if (first) {
        first = false;
      } else {
        result += " ";
      }
      result += Printer(options_, moves_to_erase_, indent_)(*m);

      if (AddDotAfterSegment(*m)) {
        bool is_previous_part_of_shift_table = ContainsOnlyShifts(*m);
        bool is_next_part_of_shift_table =
            i + 1 < move.get_content().size() &&
            ContainsOnlyShifts(*move.get_content()[i + 1]);

        if ((options_.dots_in_shifttables != DotsInShifttables::kExclude ||
             !is_previous_part_of_shift_table ||
             !is_next_part_of_shift_table) &&
            (options_.dots_in_shifttables != DotsInShifttables::kExclusively ||
             (is_previous_part_of_shift_table &&
              is_next_part_of_shift_table))) {
          result += " .";
        }
      }
    }

    result += " )";

    return result;
  }
  std::string StarCase(const rbg_parser::star_move &move) override {
    bool is_part_of_shift_table = ContainsOnlyShifts(move);
    std::string result;

    bool add_before =
        options_.add_dots_in_stars &&
        (options_.dots_in_shifttables != DotsInShifttables::kExclude ||
         !is_part_of_shift_table);
    bool add_after =
        AddDotAfterSegment(*move.get_content()) &&
        (options_.dots_in_shifttables != DotsInShifttables::kExclude ||
         !is_part_of_shift_table) &&
        (options_.dots_in_shifttables != DotsInShifttables::kExclusively ||
         is_part_of_shift_table);

    bool adding_parantheses = add_before || add_after;
    if (adding_parantheses) {
      result += "(";
    }

    if (add_before) {
      result += ". ";
    }

    result += Printer(options_, moves_to_erase_, indent_)(*move.get_content());

    if (add_after) {
      result += ". ";
    }

    if (adding_parantheses) {
      result += ")";
    }

    result += "*";

    return result;
  }

  std::string MoveCheckCase(const rbg_parser::move_check &move) override {
    std::string prefix = move.is_negated() ? "{!" : "{?";

    return prefix + "\n" + times(kIndentChars, indent_ + 1) +
           Printer({}, moves_to_erase_, indent_ + 1)(*move.get_content()) +
           "\n" + times(kIndentChars, indent_) + "}";
  }

  std::string ArithmeticExpressionCase(
      const rbg_parser::arithmetic_expression &expr) override {
    return expr.to_rbg();
  }

  std::string GameMoveCase(const rbg_parser::game_move &move) override {
    std::string result;
    if (options_.modifier_indices && move.is_modifier()) {
      result += "/*" + std::to_string(move.index_in_expression()) + "*/";
    }
    return result + trim(move.to_rbg(rbg_parser::options{}));
  }

  std::string NoopCase(const rbg_parser::noop &move) override {
    if (moves_to_erase_.find(&move) != moves_to_erase_.end()) {
      return "";
    }
    return ".";
  }

  bool AddDotAfterSegment(const rbg_parser::game_move &move) {
    if (ParserNodeType(move) == NodeType::kStar &&
        options_.add_dots_after_stars) {
      return true;
    }
    if (ParserNodeType(move) == NodeType::kSum &&
        options_.add_dots_after_alternatives) {
      return true;
    }
    return false;
  }

private:
  const PrinterOptions &options_;
  const std::unordered_set<const rbg_parser::game_move *> moves_to_erase_;
  size_t indent_;
};

int main(int argc, const char *argv[]) {
  auto args = std_ext::parse_args(argc, argv);

  if (args.positional_args.size() != 1) {
    std::cout << "Usage: " << argv[0]
              << " <game_file> [--modifier_indices true|false]"
                 "[--add_dots_in_alternatives true|false]"
                 "[--add_dots_in_stars true|false]"
                 "[--add_dots_after_alternatives true|false]"
                 "[--add_dots_after_stars true|false]"
                 "[--add_dots_after_nontrivial_shifttables true|false]"
                 "[--dots_in_shifttables exclusively|exclude|whatever]"
              << "\ndots_in_shifttables is whatever by default." << std::endl;
    return 0;
  }

  std::ifstream file_stream(args.positional_args[0]);
  std::stringstream buffer;
  buffer << file_stream.rdbuf();

  std::unique_ptr<rbg_parser::parsed_game> parsed_game;
  try {
    parsed_game = ParseGame(buffer.str());
  } catch (rbg_parser::message m) {
    std::cout << m.as_warning() << std::endl;
  }

  PrinterOptions printer_options;

  if (args.flags.find("modifier_indices") != args.flags.end()) {
    printer_options.modifier_indices =
        args.flags.at("modifier_indices") == "true";
  }

  if (args.flags.find("add_dots_in_alternatives") != args.flags.end()) {
    printer_options.add_dots_in_alternatives =
        args.flags.at("add_dots_in_alternatives") == "true";
  }

  if (args.flags.find("add_dots_in_stars") != args.flags.end()) {
    printer_options.add_dots_in_stars =
        args.flags.at("add_dots_in_stars") == "true";
  }

  if (args.flags.find("add_dots_after_alternatives") != args.flags.end()) {
    printer_options.add_dots_after_alternatives =
        args.flags.at("add_dots_after_alternatives") == "true";
  }

  if (args.flags.find("add_dots_after_stars") != args.flags.end()) {
    printer_options.add_dots_after_stars =
        args.flags.at("add_dots_after_stars") == "true";
  }

  if (args.flags.find("dots_in_shifttables") != args.flags.end()) {
    printer_options.dots_in_shifttables =
        dotsInShiftTablesOptionFromString(args.flags.at("dots_in_shifttables"));
  }

  std::stringstream result;

  result << "#board = " << parsed_game->get_board().to_rbg(true) << "\n";
  result << parsed_game->get_declarations().to_rbg() << "\n";
  result << "#rules = " << Printer(printer_options)(*parsed_game->get_moves())
         << std::endl;

  std::cout << result.str();
}
