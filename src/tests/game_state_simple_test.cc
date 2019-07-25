//
// Created by shrum on 28.06.19.
//

#include <game_state/construction/gamestate_creator.h>
#include <iostream>

using namespace rbg;
using namespace std;

const char *kSmallGame = R"LIM(
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r]
    [e,e])

//   Board indices:
//   1 2
//   3 4

//       1      2       3       4  5       6       7        8      9     10  11  12    13  14  15
#rules = ->red (right + (down)* up down + (right + down)* + down + right {r} [e] down) {e} [r] ->>
)LIM";

int main() {
  auto game = CreateGameState(kSmallGame);
  auto first_player_moves = game.Moves();
  assert(first_player_moves.size() == 3);
  std::unordered_set<std::string> expected;
  for (const auto &move : first_player_moves) {
    assert(move.size() > 1);
    if (move[0].vertex == 3) {
      assert(move.size() == 2);
      assert(move[0].modifier_index == 14);
      assert(move[1].vertex == 3);
      assert(move[1].modifier_index == 15);
      expected.insert("down [r]");
    } else if (move[0].vertex == 2) {
      assert(move.size() == 3);
      assert(move[0].modifier_index == 11);
      assert(move[1].vertex == 4);
      assert(move[1].modifier_index == 14);
      assert(move[2].vertex == 4);
      assert(move[2].modifier_index == 15);
      expected.insert("right [e] down [r]");
    } else if (move[0].vertex == 4) {
      assert(move.size() == 2);
      assert(move[0].modifier_index == 14);
      assert(move[1].vertex == 4);
      assert(move[1].modifier_index == 15);
      expected.insert("right down [r]");
    }
  }
  assert(expected.size() == 3);

  for (const auto &m : first_player_moves) {
    bool first_printed = false;
    for (const auto &e : m) {
      if (first_printed) {
        cout << ", ";
      } else {
        first_printed = true;
      }
      cout << e.vertex << " (" << game.declarations().board_description.vertices_names().Name(e.vertex) << ")"
           << " "
           << e.modifier_index;
    }
    cout << "\n";
  }
}