// We define a redundant dot as a dot whose application is fully determined by
// an application of a modifier or some other dot before or after it. This app
// will mark such dots.
#include "game_state/construction/game_state_creator.h"
#include "stl_extension/argparse.h"
#include "utility/printer.h"
#include "utility/redundant_dots.h"

#include <cassert>
#include <chrono>
#include <fstream>
#include <queue>
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

class DeterminedDotsPrinter : public AstFunction<std::string> {
public:
  DeterminedDotsPrinter(
      const std::unordered_map<const rbg_parser::game_move *,
                               NoopDeterminedState> &dots_determined_state,
      int indent = 0)
      : dots_determined_state_(dots_determined_state), indent_(indent) {}
  std::string SumCase(const rbg_parser::sum &move) override {
    if (move.get_content().size() == 1) {
      return DeterminedDotsPrinter(dots_determined_state_,
                               indent_)(*move.get_content().front());
    }

    std::string result;

    result += "(";
    result += "\n" + times(kIndentChars, indent_ + 1);

    bool first = true;
    for (const auto &m : move.get_content()) {
      if (first) {
        first = false;
      } else {
        result += "+";
        result += "\n" + times(kIndentChars, indent_ + 1);
      }

      result += DeterminedDotsPrinter(dots_determined_state_, indent_ + 1)(*m);
    }

    result += "\n" + times(kIndentChars, indent_);
    result += ")";

    return result;
  }

  std::string
  ConcatenationCase(const rbg_parser::concatenation &move) override {
    if (move.get_content().size() == 1) {
      return DeterminedDotsPrinter(dots_determined_state_,
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
      result += DeterminedDotsPrinter(dots_determined_state_, indent_)(*m);
    }

    result += " )";

    return result;
  }
  std::string StarCase(const rbg_parser::star_move &move) override {
    std::string result;
    result +=
        DeterminedDotsPrinter(dots_determined_state_, indent_)(*move.get_content());
    result += "*";
    return result;
  }

  std::string MoveCheckCase(const rbg_parser::move_check &move) override {
    std::string prefix = move.is_negated() ? "{!" : "{?";
    return prefix + "\n" + times(kIndentChars, indent_ + 1) +
           DeterminedDotsPrinter(dots_determined_state_,
                             indent_ + 1)(*move.get_content()) +
           "\n" + times(kIndentChars, indent_) + "}";
  }

  std::string ArithmeticExpressionCase(
      const rbg_parser::arithmetic_expression &expr) override {
    return expr.to_rbg();
  }

  std::string GameMoveCase(const rbg_parser::game_move &move) override {
    return trim(move.to_rbg(rbg_parser::options{}));
  }

  std::string NoopCase(const rbg_parser::noop &move) override {
    if (dots_determined_state_.find(&move) != dots_determined_state_.end()) {
      switch (dots_determined_state_.at(&move)) {
      case NoopDeterminedState::FULLY_DETERMINED:
        return "/*B*/.";
      case NoopDeterminedState::LEFT_DETERMINED:
        return "/*L*/.";
      case NoopDeterminedState::RIGHT_DETERMINED:
        return "/*R*/.";
      case NoopDeterminedState::NON_DETERMINED:
        return ".";
      }
    }
    return ".";
  }

private:
  const std::unordered_map<const rbg_parser::game_move *, NoopDeterminedState>
      &dots_determined_state_;
  size_t indent_;
};

int main(int argc, const char *argv[]) {
  auto args = std_ext::parse_args(argc, argv);

  if (args.positional_args.size() != 1) {
    std::cout << "Usage: " << argv[0] << " <game_file> [--modifiers_as_dots true|false]" << std::endl;
    return 0;
  }

  bool modifiers_as_dots = true;
  if (args.flags.find("modifiers_as_dots") != args.flags.end()) {
    if (args.flags.at("modifiers_as_dots") == "false") {
      modifiers_as_dots = false;
    }
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

  Declarations declarations = Declarations(*parsed_game);
  auto nfa =
      CreateNfa(*parsed_game->get_moves(), declarations, /*optimize=*/false);
  std::unordered_map<const rbg_parser::game_move *, NoopDeterminedState>
      noops_determined_state = FindRedundantNoops(nfa, declarations, modifiers_as_dots);

  std::stringstream result;

  result << "#board = " << parsed_game->get_board().to_rbg(true) << "\n";
  result << parsed_game->get_declarations().to_rbg() << "\n";
  result << "#rules = "
         << DeterminedDotsPrinter(noops_determined_state)(*parsed_game->get_moves())
         << std::endl;

  std::cout << result.str();
}