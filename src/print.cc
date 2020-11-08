//
// Created by shrum on 22/07/19.
//

#include <cassert>
#include <fstream>
#include <chrono>
#include <random>

#include <game_state/construction/game_state_creator.h>
#include <utility/calculate_perft.h>
#include <utility/printer.h>
#include <utility/shift_table_checker.h>
#include <stl_extension/argparse.h>
#include <networking/socket.h>

using namespace rbg;
using namespace std;

const char *kIndentChars = "  ";

std::string times(const std::string &x, size_t n)
{
  std::string result;
  for (size_t i = 0; i < n; i++)
  {
    result += x;
  }
  return result;
}

std::string trim(std::string s)
{
  s.erase(s.find_last_not_of(" \n\r\t") + 1);
  s.erase(0, s.find_first_not_of(" \n\r\t"));
  return s;
}

struct PrinterOptions
{
  bool print_numbers = false;                      // print the indices modifiers in a neat comment (ignored in conditionals)
  bool add_dots_in_alternatives = false;           // add dots between each alternative element (ignored in conditionals)
  bool add_dots_in_stars = false;                  // add dots inside star statements (ignored in conditionals)
  bool add_dots_after_alternatives = false;        // add dots after alternative (ignored in conditionals)
  bool add_dots_after_stars = false;               // add dots after stars (ignored in conditionals)
  bool disable_adding_dots_in_shifttables = false; // if add_dots_in_alternatives is (ignored in conditionals)
};

class Printer : public AstFunction<std::string>
{
public:
  Printer(const PrinterOptions &options, size_t indent = 0) : options_(options), indent_(indent) {}
  std::string SumCase(const rbg_parser::sum &move) override
  {

    if (move.get_content().size() == 1)
    {
      return Printer(options_, indent_)(*move.get_content().front());
    }

    std::string result;

    result += "(";
    result += "\n" + times(kIndentChars, indent_ + 1);

    bool first = true;
    bool is_part_of_shift_table = ContainsOnlyShifts(move);
    for (const auto &m : move.get_content())
    {
      if (first)
      {
        first = false;
      }
      else
      {
        result += "+";
        result += "\n" + times(kIndentChars, indent_ + 1);
      }
      if (options_.add_dots_in_alternatives && (!options_.disable_adding_dots_in_shifttables || !is_part_of_shift_table))
      {
        result += " .";
      }
      result += Printer(options_, indent_ + 1)(*m);
    }

    result += "\n" + times(kIndentChars, indent_);
    result += ")";

    if(options_.add_dots_after_alternatives) {
      result += " .";
    }

    return result;
  }

  std::string ConcatenationCase(const rbg_parser::concatenation &move) override
  {
    if (move.get_content().size() == 1)
    {
      return Printer(options_, indent_)(*move.get_content().front());
    }

    std::string result;

    result += "( ";

    bool first = true;
    for (size_t i = 0; i < move.get_content().size(); i++)
    {
      const auto &m = move.get_content()[i];
      if (first)
      {
        first = false;
      }
      else
      {
        result += " ";
      }
      result += Printer(options_, indent_)(*m);

      if(result.back() == '*') {
        bool is_previous_part_of_shift_table = ContainsOnlyShifts(*m);
        bool is_next_part_of_shift_table = i + 1 < move.get_content().size() && ContainsOnlyShifts(*move.get_content()[i+1]);

        if(options_.add_dots_after_stars && (!options_.disable_adding_dots_in_shifttables || !is_previous_part_of_shift_table || !is_next_part_of_shift_table)) {
          result += " .";
        }
      }
    }

    result += " )";

    return result;
  }
  std::string StarCase(const rbg_parser::star_move &move) override
  {
    bool is_part_of_shift_table = ContainsOnlyShifts(move);
    std::string result;

    if(options_.add_dots_in_stars && (!options_.disable_adding_dots_in_shifttables || !is_part_of_shift_table)) {
      result += "( . ";
    }

    result += Printer(options_, indent_)(*move.get_content());

    if(options_.add_dots_in_stars && (!options_.disable_adding_dots_in_shifttables || !is_part_of_shift_table)) {
      result += ")";
    }

    // The star has to be the last character of a result here as we are cheking that in ConcatenationCase
    result += "*";

    return result;
  }

  std::string MoveCheckCase(const rbg_parser::move_check &move) override
  {
    std::string prefix = move.is_negated() ? "{!" : "{?";
    PrinterOptions condition_options = options_;

    // Resetting stuff that is not important in conditionals
    condition_options.print_numbers = false;
    condition_options.add_dots_in_alternatives = false;
    condition_options.disable_adding_dots_in_shifttables = false;
    return prefix + "\n" + times(kIndentChars, indent_ + 1) + Printer(condition_options, indent_ + 1)(*move.get_content()) + "\n" + times(kIndentChars, indent_) + "}";
  }

  std::string ArithmeticExpressionCase(const rbg_parser::arithmetic_expression &expr) override { return expr.to_rbg(); }

  std::string GameMoveCase(const rbg_parser::game_move &move) override
  {
    std::string result;
    if (options_.print_numbers && move.is_modifier())
    {
      result += "/*" + std::to_string(move.index_in_expression()) + "*/";
    }
    return result + trim(move.to_rbg(rbg_parser::options{}));
  }

  std::string NoopCase(const rbg_parser::noop &) override { return "."; }

private:
  const PrinterOptions &options_;
  size_t indent_;
};

int main(int argc, const char *argv[])
{
  auto args = std_ext::parse_args(argc, argv);

  if (args.positional_args.size() != 1)
  {
    std::cout << "Usage: " << argv[0] << " <game_file> [--modifier_indices true|false]"
                                         "[--add_dots_in_alternatives true|false]"
                                         "[--disable_adding_dots_in_shifttables true|false]"
                                         "[--add_dots_in_stars true|false]"
                                         "[--add_dots_after_alternatives true|false]"
                                         "[--add_dots_after_stars true|false]"
              << std::endl;
    return 0;
  }

  std::ifstream file_stream(args.positional_args[0]);
  std::stringstream buffer;
  buffer << file_stream.rdbuf();

  auto parsed_game = ParseGame(buffer.str());

  PrinterOptions printer_options;

  if (args.flags.find("modifier_indices") != args.flags.end())
  {
    printer_options.print_numbers = args.flags.at("modifier_indices") == "true";
  }

  if (args.flags.find("add_dots_in_alternatives") != args.flags.end())
  {
    printer_options.add_dots_in_alternatives = args.flags.at("add_dots_in_alternatives") == "true";
  }

  if (args.flags.find("disable_adding_dots_in_shifttables") != args.flags.end())
  {
    printer_options.disable_adding_dots_in_shifttables = args.flags.at("disable_adding_dots_in_shifttables") == "true";
  }

  if (args.flags.find("add_dots_in_stars") != args.flags.end())
  {
    printer_options.add_dots_in_stars = args.flags.at("add_dots_in_stars") == "true";
  }

  if (args.flags.find("add_dots_after_alternatives") != args.flags.end())
  {
    printer_options.add_dots_after_alternatives = args.flags.at("add_dots_after_alternatives") == "true";
  }

  if (args.flags.find("add_dots_after_stars") != args.flags.end())
  {
    printer_options.add_dots_after_stars = args.flags.at("add_dots_after_stars") == "true";
  }


  std::cout << "#board = " << parsed_game->get_board().to_rbg(true) << "\n";
  std::cout << parsed_game->get_declarations().to_rbg() << "\n";
  std::cout << "#rules = " << Printer(printer_options)(*parsed_game->get_moves()) << std::endl;
}
