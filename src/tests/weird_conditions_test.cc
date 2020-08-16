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

  const char *kWeirdConditionsTest = R"LIM(
  // Weird one

  #players = one(100)
  #variables =
  #pieces = e, o, x

  #board = rectangle(up,down,left,right,
  [x,x,o,o,o,o,o,o,o,o,o,o,o,o,o,x,x,x,x]
  [x,x,x,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,x]
  [x,x,x,x,x,o,o,o,o,o,o,o,o,o,o,o,o,x,x]
  [x,x,x,x,o,o,o,o,o,o,o,o,o,o,o,o,o,x,x]
  [x,x,x,x,o,o,o,o,x,x,o,o,o,o,o,o,o,x,x]
  [x,x,o,o,o,o,o,x,x,e,x,o,o,o,o,o,x,x,x]
  [x,x,x,x,o,o,o,x,o,x,o,o,o,o,o,x,x,x,x]
  [x,x,o,o,o,o,o,o,o,o,o,o,o,o,o,x,x,x,x]
  [o,x,o,o,o,o,x,o,o,e,o,o,o,o,o,o,x,x,x]
  [o,o,o,o,o,o,x,x,x,o,x,o,o,o,o,x,x,x,x]
  [o,o,o,o,o,x,x,x,x,o,x,x,o,o,o,x,x,x,x]
  [o,o,o,o,o,o,x,x,x,x,x,x,x,o,o,x,x,x,x]
  [o,o,x,x,o,o,x,x,x,x,x,x,x,o,x,x,x,x,x]
  [o,o,o,x,o,x,x,x,x,x,x,x,x,x,x,x,x,x,x]
  [o,o,o,x,o,x,x,x,o,o,x,x,x,x,x,x,x,x,x]
  [o,o,x,x,x,x,x,x,o,o,o,x,x,x,x,x,x,x,x]
  [x,x,x,x,o,o,x,x,o,o,x,x,x,x,x,o,x,x,x]
  [x,e,x,x,x,o,o,o,o,o,x,x,x,o,o,e,o,x,x] // <- we start here and go through x's to e on the left
  [x,x,x,x,o,o,o,o,o,x,x,x,x,x,x,o,o,o,o])

  #anySquare = (left* + right*)(up* + down*)
  #anyNeighbour(neighbour) = ((up+down+left+right){neighbour})

  #rules = ->one 
      anySquare 
      {e} [o]
      (up + down)
      {o}
      {! anyNeighbour(x)* anyNeighbour(e) }
      ->one
  )LIM";
}

int main() {
  try {
      auto game = CreateGameState(kWeirdConditionsTest);
      auto moves = game.Moves();
      assert(moves.size() == 0);
      std::cout << RectangularBoardDescription(game.board_content(), game.declarations());
      std::cout << "There should be no moves." << std::endl;
  }
  catch(rbg_parser::message& m){
    std::cout<<m.as_error()<<std::endl;
  }
  catch(std::exception& e){
    std::cout<<e.what()<<std::endl;
  }
}
 
