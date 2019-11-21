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

const char* kIndentChars = "  ";

std::string times(const std::string& x, size_t n) {
  std::string result;
  for(size_t i = 0; i < n; i++) {
    result += x;
  }
  return result;
}

std::string trim(std::string s) {
  s.erase(s.find_last_not_of(" \n\r\t")+1);
  s.erase(0, s.find_first_not_of(" \n\r\t"));
  return s;
}

class Printer : public AstFunction<std::string> {
public:
  Printer(size_t indent=0, bool print_numbers=true) : indent_(indent), print_numbers_(print_numbers) {}
  virtual std::string SumCase(const rbg_parser::sum &move) {
    std::string result;

    if(move.get_content().size()>1) {
      result += "(";
      result += "\n" + times(kIndentChars,indent_+1);
    }

    bool first = true;
    for(const auto& m : move.get_content()) {
      if(first) {
        first = false;
      } else {
        result += "+";
        result += "\n" + times(kIndentChars,indent_+1);
      }
      result += Printer(indent_+1, print_numbers_)(*m);
    }


    if(move.get_content().size()>1) {
      result += "\n" + times(kIndentChars,indent_);
      result += ")";
    }

    return result;
  }

  virtual std::string ConcatenationCase(const rbg_parser::concatenation &move) {
    std::string result;

    if(move.get_content().size()>1) {
      result += "( ";
    }

    bool first = true;
    for(const auto& m : move.get_content()) {
      if(first) {
        first = false;
      } else {
        result += " ";
      }
      result += Printer(indent_, print_numbers_)(*m);
    }

    if(move.get_content().size()>1) {
      result += " )";
    }
    return result;
  }
  virtual std::string StarCase(const rbg_parser::star_move &move) {
    std::string result;
    result += Printer(indent_, print_numbers_)(*move.get_content()) +"*";
    return result;
  }

  virtual std::string MoveCheckCase(const rbg_parser::move_check &move) {
    std::string prefix = move.is_negated() ? "{!" : "{?";
    return  prefix + "\n" + times(kIndentChars,indent_+1) + Printer(indent_+1, false)(*move.get_content()) + "\n" + times(kIndentChars, indent_) + "}";
  }

  virtual std::string ArithmeticExpressionCase(const rbg_parser::arithmetic_expression &expr) { return expr.to_rbg(); }

  virtual std::string GameMoveCase(const rbg_parser::game_move &move) {
    std::string result;
    if(print_numbers_) {
      result += "/*" + std::to_string(move.index_in_expression()) +"*/";
    }
    return result + trim(move.to_rbg()); }

private:
  size_t indent_;
  bool print_numbers_;
};

int main(int argc, const char *argv[]) {
  auto args = std_ext::parse_args(argc, argv);
  
  if (args.positional_args.size() != 1) {
    std::cout << "Usage: " << argv[0] << " <game_file> --style (original|original_pretty|new)" << std::endl;
    return 0;
  }

  std::ifstream file_stream(args.positional_args[0]);
  std::stringstream buffer;
  buffer << file_stream.rdbuf();

  auto parsed_game = ParseGame(buffer.str());
  std::string style = "new";
  if(args.flags.find("style") != args.flags.end()) {
    style = args.flags.at("style");
  }
  if(style == "original") {
    std::cout << parsed_game->to_rbg(false);
  } else if (style == "original_pretty") {
    std::cout << parsed_game->to_rbg(true);
  } else {
    std::cout << "#board = " << parsed_game->get_board().to_rbg(true) << "\n";
    std::cout << parsed_game->get_declarations().to_rbg() << "\n";
    std::cout << "#rules = " << Printer()(*parsed_game->get_moves()) << std::endl;
  }

}
