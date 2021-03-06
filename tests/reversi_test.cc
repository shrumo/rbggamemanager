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

const char *kReversiGame = R"LIM(
// Othello (reversi)

#players = white(100), black(100)
#pieces = e, w, b
#variables =
#board = rectangle(up,down,left,right,
         [e,e,e,e,e,e,e,e]
         [e,e,e,e,e,e,e,e]
         [e,e,e,e,e,e,e,e]
         [e,e,e,w,b,e,e,e]
         [e,e,e,b,w,e,e,e]
         [e,e,e,e,e,e,e,e]
         [e,e,e,e,e,e,e,e]
         [e,e,e,e,e,e,e,e])

#anySquare = ((up* + down*)(left* + right*))
#anyNeighborSquare = (up + down + left + right + (up + down)(left + right))

#swapDirectionPossible(mePawn; oppPawn; dir) = (
    dir {oppPawn} dir
    ({oppPawn} dir)*
    {mePawn}
  )

#movePossible(mePawn; oppPawn) = (
      swapDirectionPossible(mePawn; oppPawn; up left)
    + swapDirectionPossible(mePawn; oppPawn; up)
    + swapDirectionPossible(mePawn; oppPawn; up right)
    + swapDirectionPossible(mePawn; oppPawn; left)
    + swapDirectionPossible(mePawn; oppPawn; right)
    + swapDirectionPossible(mePawn; oppPawn; down left)
    + swapDirectionPossible(mePawn; oppPawn; down)
    + swapDirectionPossible(mePawn; oppPawn; down right)
  )

#swapDirection(mePawn; oppPawn; dir; backDir) = (
      {! swapDirectionPossible(mePawn; oppPawn; dir)}
    + dir {oppPawn} [mePawn] dir ({oppPawn} [mePawn] dir)* {mePawn}
      backDir ({mePawn} backDir)* {e}
  )

#swapDirections(mePawn; oppPawn) = (
    swapDirection(mePawn; oppPawn; up left; down right)
    swapDirection(mePawn; oppPawn; up; down)
    swapDirection(mePawn; oppPawn; up right; down left)
    swapDirection(mePawn; oppPawn; left; right)
    swapDirection(mePawn; oppPawn; right; left)
    swapDirection(mePawn; oppPawn; down left; up right)
    swapDirection(mePawn; oppPawn; down; up)
    swapDirection(mePawn; oppPawn; down right; up left)
  )

#setScores = (
      {$ w > b} [$ white = 100]
    + {$ w < b} [$ black = 100]
    + {$ w == b} [$ black = 50] [$ white = 50]
  )

#turn(me; mePawn; opp; oppPawn) = (
    ->me (
        (anySquare {oppPawn} anyNeighborSquare {e} {? movePossible(mePawn; oppPawn)})
        ->>
        swapDirections(mePawn; oppPawn)
        [mePawn]
      + {! anySquare {oppPawn} anyNeighborSquare {e} {? movePossible(mePawn; oppPawn)}}
        ->>
    )
    (
        {? anySquare ({oppPawn} anyNeighborSquare {e} movePossible(mePawn; oppPawn) + {mePawn} anyNeighborSquare {e} movePossible(oppPawn; mePawn))}
      + {! anySquare ({oppPawn} anyNeighborSquare {e} movePossible(mePawn; oppPawn) + {mePawn} anyNeighborSquare {e} movePossible(oppPawn; mePawn))}
        ->> setScores ->> {}
    )
  )

#rules = (
    turn(black; b; white; w)
    turn(white; w; black; b)
  )*
)LIM";
}  // namespace

int main() {
  auto game = CreateGameState(kReversiGame);
  auto result = Perft(game, 4);
  assert(result.leaves_count == 244);
  assert(result.nodes_count == 317);
  game.Apply(game.Moves()[0]);
  std::cout << RectangularBoardDescription(game.board_content(),
                                           game.declarations());
}