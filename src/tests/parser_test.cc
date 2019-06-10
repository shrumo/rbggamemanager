//
// Created by shrum on 03.06.19.
//
#include <parser/parser_wrapper.h>
#include <cassert>

using namespace rbg;

const char *kSmallGame = R"LIM(
#players = red(100), blue(100)
#pieces = e
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [e,e]
    [e,e])
//       1       2    3      4      5        6                 7            8   9
#rules = ->red ( up + down + left + right )* [$ turn = turn+1] {$turn < 10} [e] ->blue
)LIM";

// Visitor that counts the number of nodes in the ast.
class ActionsCount : public AstFunction<int> {
  int SumCase(const rbg_parser::sum &move) override {
    int result = 0;
    for (const auto &child : move.get_content()) {
      result += ActionsCount()(*child);
    }
    return result;
  }

  int ConcatenationCase(const rbg_parser::concatenation &move) override {
    int result = 0;
    for (const auto &child : move.get_content()) {
      result += ActionsCount()(*child);
    }
    return result;
  }

  int StarCase(const rbg_parser::star_move &move) override {
    int result = 0;
    result += ActionsCount()(*move.get_content());
    return result;
  }

  int GameMoveCase(const rbg_parser::game_move &) override {
    return 1;
  }

  int DefaultCase() override {
    throw std::logic_error("This shouldn't match. We are not visiting arithmetic nodes.");
  }
};

int main() {
  auto pg = ParseGame(kSmallGame);
  assert(ActionsCount()(*pg->get_moves()) == 9);
}