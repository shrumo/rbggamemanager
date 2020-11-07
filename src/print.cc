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
  bool print_numbers; // print the indices modifiers in a neat comment
  bool add_dots_in_alternatives; // whether to add dots between each alternative element
};

class Printer : public AstFunction<std::string>
{
public:
  Printer(const PrinterOptions &options, size_t indent = 0) : options_(options), indent_(indent) {}
  virtual std::string SumCase(const rbg_parser::sum &move)
  {

    if (move.get_content().size() == 1)
    {
      return Printer(options_, indent_)(*move.get_content().front());
    }

    std::string result;

    result += "(";
    result += "\n" + times(kIndentChars, indent_ + 1);

    bool first = true;
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
      if(options_.add_dots_in_alternatives) {
        result += ".";
      }
      result += Printer(options_, indent_ + 1)(*m);
    }

    result += "\n" + times(kIndentChars, indent_);
    result += ")";

    return result;
  }

  virtual std::string ConcatenationCase(const rbg_parser::concatenation &move)
  {
    if (move.get_content().size() == 1)
    {
      return Printer(options_, indent_)(*move.get_content().front());
    }

    std::string result;

    result += "( ";

    bool first = true;
    for (const auto &m : move.get_content())
    {
      if (first)
      {
        first = false;
      }
      else
      {
        result += " ";
      }
      result += Printer(options_, indent_)(*m);
    }

    result += " )";

    return result;
  }
  virtual std::string StarCase(const rbg_parser::star_move &move)
  {
    std::string result;
    result += Printer(options_, indent_)(*move.get_content()) + "*";
    return result;
  }

  virtual std::string MoveCheckCase(const rbg_parser::move_check &move)
  {
    std::string prefix = move.is_negated() ? "{!" : "{?";
    // We don't want to print the modifier indices inside conditional expressions, because they are not relevant
    PrinterOptions condition_options = options_;
    condition_options.print_numbers = false;
    return prefix + "\n" + times(kIndentChars, indent_ + 1) + Printer(condition_options, indent_ + 1)(*move.get_content()) + "\n" + times(kIndentChars, indent_) + "}";
  }

  virtual std::string ArithmeticExpressionCase(const rbg_parser::arithmetic_expression &expr) { return expr.to_rbg(); }

  virtual std::string GameMoveCase(const rbg_parser::game_move &move)
  {
    std::string result;
    if (options_.print_numbers && move.is_modifier())
    {
      result += "/*" + std::to_string(move.index_in_expression()) + "*/";
    }
    return result + trim(move.to_rbg(rbg_parser::options{}));
  }

  virtual std::string NoopCase(const rbg_parser::noop &) { return "."; }

private:
  const PrinterOptions &options_;
  size_t indent_;
};

int main(int argc, const char *argv[])
{
  auto args = std_ext::parse_args(argc, argv);

  if (args.positional_args.size() != 1)
  {
    std::cout << "Usage: " << argv[0] << " <game_file> [--modifier_indices true|false] [--add_dots_in_alternatives true|false]" << std::endl;
    return 0;
  }

  std::ifstream file_stream(args.positional_args[0]);
  std::stringstream buffer;
  buffer << file_stream.rdbuf();

  auto parsed_game = ParseGame(buffer.str());

  PrinterOptions printer_options;

  if (args.flags.find("modifier_indices") != args.flags.end())
  {
    if (args.flags.at("modifier_indices") == "true")
    {
      printer_options.print_numbers = true;
    }
  }

  if (args.flags.find("add_dots_in_alternatives") != args.flags.end())
  {
    if (args.flags.at("add_dots_in_alternatives") == "true")
    {
      printer_options.add_dots_in_alternatives = true;
    }
  }

  std::cout << "#board = " << parsed_game->get_board().to_rbg(true) << "\n";
  std::cout << parsed_game->get_declarations().to_rbg() << "\n";
  std::cout << "#rules = " << Printer(printer_options)(*parsed_game->get_moves()) << std::endl;
}
