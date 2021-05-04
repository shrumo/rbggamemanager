// We define an illegal dot as a dot in the rules that would increase the number
// of moves if it was treated as a modifier. This app will mark such dots.
#include "game_state/construction/game_state_creator.h"
#include "stl_extension/argparse.h"
#include "utility/illegal_dots.h"
#include "utility/printer.h"

#include <cassert>
#include <chrono>
#include <fstream>
#include <queue>
#include <random>

using namespace rbg;
using namespace std;

// Moving dots code below

std::string trim(std::string s) {
  s.erase(s.find_last_not_of(" \n\r\t") + 1);
  s.erase(0, s.find_first_not_of(" \n\r\t"));
  return s;
}

struct DotsMoverPrinterResult {
  std::string text;
  bool should_insert_dot;
};

class DotsMoverPrinter : public AstFunction<DotsMoverPrinterResult> {
public:
  DotsMoverPrinter(
      const unordered_set<const rbg_parser::game_move *> &dots_to_mark)
      : dots_to_move_(dots_to_mark) {}
  DotsMoverPrinterResult SumCase(const rbg_parser::sum &move) override {
    if (move.get_content().size() == 1) {
      return DotsMoverPrinter(dots_to_move_)(*move.get_content().front());
    }

    std::string text;

    text += "(";
    bool should_insert_dot = false;

    bool first = true;
    for (const auto &m : move.get_content()) {
      if (first) {
        first = false;
      } else {
        text += "+";
      }

      DotsMoverPrinterResult recursive_result =
          DotsMoverPrinter(dots_to_move_)(*m);
      text += recursive_result.text;
      should_insert_dot =
          should_insert_dot || recursive_result.should_insert_dot;
    }

    text += ")";

    return {text, should_insert_dot};
  }

  DotsMoverPrinterResult
  ConcatenationCase(const rbg_parser::concatenation &move) override {
    if (move.get_content().size() == 1) {
      return DotsMoverPrinter(dots_to_move_)(*move.get_content().front());
    }

    std::string text;

    text += "( ";

    bool first = true;
    bool insert_dot_after_next = false;
    for (size_t i = 0; i < move.get_content().size(); i++) {
      const auto &m = move.get_content()[i];
      if (first) {
        first = false;
      } else {
        text += " ";
      }
      DotsMoverPrinterResult recursive_result =
          DotsMoverPrinter(dots_to_move_)(*m);

      if (recursive_result.should_insert_dot) {
        if (trim(recursive_result.text) == "") {
          insert_dot_after_next = true;
          continue;
        }
      }

      text += recursive_result.text;
      if (recursive_result.should_insert_dot || insert_dot_after_next) {
        text += ". ";
        insert_dot_after_next = false;
      }
    }

    text += " )";

    return {text, /*should_insert_dot=*/insert_dot_after_next};
  }
  DotsMoverPrinterResult StarCase(const rbg_parser::star_move &move) override {
    std::string text;
    DotsMoverPrinterResult recursive_result =
        DotsMoverPrinter(dots_to_move_)(*move.get_content());
    text += recursive_result.text;
    text += "*";
    if (recursive_result.should_insert_dot) {
      text += " .";
    }
    return {text, false};
  }

  DotsMoverPrinterResult
  MoveCheckCase(const rbg_parser::move_check &move) override {
    std::string prefix = move.is_negated() ? "{!" : "{?";
    return {prefix + "\n" +
                DotsMoverPrinter(dots_to_move_)(*move.get_content()).text +
                "\n" + "}",
            /*should_insert_dot=*/false};
  }

  DotsMoverPrinterResult ArithmeticExpressionCase(
      const rbg_parser::arithmetic_expression &expr) override {
    return {expr.to_rbg(), /*should_insert_dot=*/false};
  }

  DotsMoverPrinterResult
  GameMoveCase(const rbg_parser::game_move &move) override {
    return {trim(move.to_rbg(rbg_parser::options{})),
            /*should_insert_dot=*/false};
  }

  DotsMoverPrinterResult NoopCase(const rbg_parser::noop &move) override {
    if (dots_to_move_.find(&move) != dots_to_move_.end()) {
      return {"", /*should_insert_dot=*/true};
    }
    return {".", /*should_insert_dot=*/false};
  }

private:
  const std::unordered_set<const rbg_parser::game_move *> dots_to_move_;
};

std::string MoveIllegalDotsToFirstLegalSpaces(const std::string &game_text) {
  std::stringstream result;
  result << game_text;

  unordered_set<const rbg_parser::game_move *> illegal_noop_moves;
  do {
    std::unique_ptr<rbg_parser::parsed_game> parsed_game;

    parsed_game = ParseGame(result.str());

    Declarations declarations = Declarations(*parsed_game);
    auto nfa =
        CreateNfa(*parsed_game->get_moves(), declarations, /*optimize=*/false);
    illegal_noop_moves = FindIllegalNoops(nfa, declarations);

    result.str("");
    result.clear();
    result << "#board = " << parsed_game->get_board().to_rbg(true) << "\n";
    result << parsed_game->get_declarations().to_rbg() << "\n";
    result
        << "#rules = "
        << DotsMoverPrinter(illegal_noop_moves)(*parsed_game->get_moves()).text
        << std::endl;
  } while (illegal_noop_moves.size() > 0);
  return result.str();
}

int main(int argc, const char *argv[]) {

  auto args = std_ext::parse_args(argc, argv);

  if (args.positional_args.size() != 1) {
    std::cout << "Usage: " << argv[0] << " <game_file>" << std::endl;
    return 0;
  }

  std::ifstream file_stream(args.positional_args[0]);
  std::stringstream buffer;
  buffer << file_stream.rdbuf();

  std::cout << MoveIllegalDotsToFirstLegalSpaces(buffer.str());
}