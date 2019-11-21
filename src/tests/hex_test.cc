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

  const char *kHexGame = R"LIM(
// Hex

#players = red(100), blue(100)
#pieces = e, r, b
#variables =
#board = hexagon(NW, NE, E, SE, SW, W,
                      [ e ]
                    [ e , e ]
                  [ e , e , e ]
                [ e , e , e , e ]
              [ e , e , e , e , e ]
            [ e , e , e , e , e , e ]
          [ e , e , e , e , e , e , e ]
        [ e , e , e , e , e , e , e , e ]
      [ e , e , e , e , e , e , e , e , e ]
    [ e , e , e , e , e , e , e , e , e , e ]
  [ e , e , e , e , e , e , e , e , e , e , e ]
    [ e , e , e , e , e , e , e , e , e , e ]
      [ e , e , e , e , e , e , e , e , e ]
        [ e , e , e , e , e , e , e , e ]
          [ e , e , e , e , e , e , e ]
            [ e , e , e , e , e , e ]
              [ e , e , e , e , e ]
                [ e , e , e , e ]
                  [ e , e , e ]
                    [ e , e ]
                      [ e ])

#anyNeighbor = (NW + NE + E + SE + SW + W)
#anySquare = anyNeighbor*

#reachable(dir; piece) = ((anyNeighbor {piece})* {! dir})

#turn(me; opp; dir1; dir2; piece) = (
    ->me anySquare {e} ->> [piece]
    (
        ({! reachable(dir1; piece)} + {! reachable(dir2; piece)})
      + {? reachable(dir1; piece)} {? reachable(dir2; piece)}
        [$ me=100] ->> {}
    )
  )

#rules = (
    turn(red; blue; NW; SE; r)
    turn(blue; red; NE; SW; b)
  )*
)LIM";
}

int main() {
  auto game = CreateGameState(kHexGame);
  auto result = Perft(game, 3);
  assert(result.leaves_count == 1727880);
  assert(result.nodes_count == 1742522);
  game.Apply(game.Moves()[0]);
  std::cout << RectangularBoardDescription(game.board_content(), game.declarations());
}