// We define a redundant dot as a dot whose application is fully determined by
// an application of a modifier or some other dot before or after it. This app
// will remove such dots.
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

class DeterminedDotsEraserPrinter : public AstFunction<std::string> {
public:
  DeterminedDotsEraserPrinter(
      const std::unordered_map<const rbg_parser::game_move *,
                               NoopDeterminedState> &dots_determined_state,
      NoopDeterminedState state_to_erase)
      : dots_determined_state_(dots_determined_state),
        state_to_erase_(state_to_erase) {}
  std::string SumCase(const rbg_parser::sum &move) override {
    if (move.get_content().size() == 1) {
      return DeterminedDotsEraserPrinter(
          dots_determined_state_, state_to_erase_)(*move.get_content().front());
    }

    std::string result;

    result += "(";
    result += "\n";

    bool first = true;
    for (const auto &m : move.get_content()) {
      if (first) {
        first = false;
      } else {
        result += "+";
        result += "\n";
      }

      result += DeterminedDotsEraserPrinter(dots_determined_state_,
                                            state_to_erase_)(*m);
    }

    result += "\n";
    result += ")";

    return result;
  }

  std::string
  ConcatenationCase(const rbg_parser::concatenation &move) override {
    if (move.get_content().size() == 1) {
      return DeterminedDotsEraserPrinter(
          dots_determined_state_, state_to_erase_)(*move.get_content().front());
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
      result += DeterminedDotsEraserPrinter(dots_determined_state_,
                                            state_to_erase_)(*m);
    }

    result += " )";

    return result;
  }
  std::string StarCase(const rbg_parser::star_move &move) override {
    std::string result;
    result += DeterminedDotsEraserPrinter(dots_determined_state_,
                                          state_to_erase_)(*move.get_content());
    result += "*";
    return result;
  }

  std::string MoveCheckCase(const rbg_parser::move_check &move) override {
    std::string prefix = move.is_negated() ? "{!" : "{?";
    return prefix + "\n" +
           DeterminedDotsEraserPrinter(dots_determined_state_,
                                       state_to_erase_)(*move.get_content()) +
           "\n" + "}";
  }

  std::string ArithmeticExpressionCase(
      const rbg_parser::arithmetic_expression &expr) override {
    return expr.to_rbg();
  }

  std::string GameMoveCase(const rbg_parser::game_move &move) override {
    return trim(move.to_rbg(rbg_parser::options{}));
  }

  std::string NoopCase(const rbg_parser::noop &move) override {
    if (dots_determined_state_.find(&move) != dots_determined_state_.end() &&
       (dots_determined_state_.at(&move) == state_to_erase_ ||
       dots_determined_state_.at(&move) == NoopDeterminedState::FULLY_DETERMINED)) {
      return "";
    }
    return ".";
  }

private:
  const std::unordered_map<const rbg_parser::game_move *, NoopDeterminedState>
      &dots_determined_state_;
  NoopDeterminedState state_to_erase_;
};

int main(int argc, const char *argv[]) {
  auto args = std_ext::parse_args(argc, argv);

  if (args.positional_args.size() != 1) {
    std::cout << "Usage: " << argv[0]
              << " <game_file> [--state_to_erase left|right]"
              << "\n"
              << "The default erased state is left." << std::endl;
    return 0;
  }

  NoopDeterminedState state_to_erase = NoopDeterminedState::LEFT_DETERMINED;
  if (args.flags.find("state_to_erase") != args.flags.end()) {
    if (args.flags.at("state_to_erase") == "right") {
      state_to_erase = NoopDeterminedState::RIGHT_DETERMINED;
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
      noops_determined_state = FindRedundantNoops(nfa, declarations);

  std::stringstream result;

  result << "#board = " << parsed_game->get_board().to_rbg(true) << "\n";
  result << parsed_game->get_declarations().to_rbg() << "\n";
  result << "#rules = "
         << DeterminedDotsEraserPrinter(noops_determined_state, state_to_erase)(
                *parsed_game->get_moves())
         << std::endl;

  std::cout << result.str();
}