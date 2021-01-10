//
// Created by shrum on 30.06.19.
//

#include <game_state/construction/game_state_creator.h>
#include <utility/calculate_perft.h>
#include <utility/printer.h>

#include <iostream>

namespace {
using namespace rbg;
using namespace std;

const char *kTicTacToeGame = R"LIM(
// TicTacToe

#players = xplayer(100), oplayer(100)
#pieces = e, x, o
#variables =
#board = rectangle(up,down,left,right,
    [e, e, e]
    [e, e, e]
    [e, e, e])

#anySquare = ((up* + down*)(left* + right*))

#winAtDir(dir; oppDir; piece) = (dir {piece} (dir + (oppDir)^2) {piece} + (oppDir {piece})^2)
#winAt(piece) = (
      winAtDir(up left; down right; piece)
    + winAtDir(up; down; piece)
    + winAtDir(up right; down left; piece)
    + winAtDir(left; right; piece)
  )

#turn(me; opp) =
  ->me~player anySquare {e} . . . . ->> . [me]
  (
      {! winAt(me)}
    + {? winAt(me)} [$ me~player=100] [$ opp~player=0] ->> {}
  )

#rules = . . . . . [$ xplayer=50] [$ oplayer=50] (
    turn(x; o)
    turn(o; x)
  )*

)LIM";
}  // namespace

int main() {
  auto game = CreateGameState(kTicTacToeGame);
  auto result = Perft(game, 9);
  assert(result.leaves_count == 127872);
  assert(result.nodes_count == 549946);
  game.Apply(game.Moves()[0]);
  std::cout << RectangularBoardDescription(game.board_content(),
                                           game.declarations());
}
