//
// Created by shrum on 30.06.19.
//

#include <game_state/construction/game_state_creator.h>
#include <utility/calculate_perft.h>
#include <iostream>
#include <utility/printer.h>


namespace {
  using namespace rbg;
  using namespace std;

  const char *kAmazonsGame = R"LIM(
// Amazons

#players = white(100), black(100)
#pieces = e, w, b, x
#variables =
#board = rectangle(up,down,left,right,
         [e, e, e, b, e, e, b, e, e, e]
         [e, e, e, e, e, e, e, e, e, e]
         [e, e, e, e, e, e, e, e, e, e]
         [b, e, e, e, e, e, e, e, e, b]
         [e, e, e, e, e, e, e, e, e, e]
         [e, e, e, e, e, e, e, e, e, e]
         [w, e, e, e, e, e, e, e, e, w]
         [e, e, e, e, e, e, e, e, e, e]
         [e, e, e, e, e, e, e, e, e, e]
         [e, e, e, w, e, e, w, e, e, e])

#anySquare = ((up* + down*)(left* + right*))
#directedShift(dir) = (dir {e} (dir {e})*)

#queenShift = (
    directedShift(up left) +
    directedShift(up) +
    directedShift(up right) +
    directedShift(left) +
    directedShift(right) +
    directedShift(down left) +
    directedShift(down) +
    directedShift(down right)
  )

#turn(piece; me; opp) = (
    ->me anySquare {piece} [e]
    queenShift [piece]
    queenShift
    ->> [x] [$ me=100, opp=0]
  )

#rules = (turn(w; white; black) turn(b; black; white))*
)LIM";
}

int main() {
  auto game = CreateGameState(kAmazonsGame);
  auto result = Perft(game, 2);
  assert(result.leaves_count == 4307152);
  assert(result.nodes_count == 4309329);
  game.Apply(game.Moves()[0]);
  std::cout << RectangularBoardDescription(game.board_content(), game.declarations());
}