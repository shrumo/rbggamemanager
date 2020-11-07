//
// Created by shrum on 30.06.19.
//

#include <game_state/construction/game_state_creator.h>
#include <utility/calculate_perft.h>
#include <iostream>
#include <utility/printer.h>
#include <networking/socket.h>

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
  ->me~player anySquare {e} ->> [me]
  (
      {! winAt(me)}
    + {? winAt(me)} [$ me~player=100] [$ opp~player=0] ->> {}
  )

#rules = [$ xplayer=50] [$ oplayer=50] (
    turn(x; o)
    turn(o; x)
  )*

)LIM";

  const char *kLabeledTicTacToeGame = R"LIM(
// Numbers of vertices
// 1 2 3
// 4 5 6
// 7 8 9
#board =
    rx0y0[e]{down:rx0y1,right:rx1y0}
    rx1y0[e]{down:rx1y1,left:rx0y0,right:rx2y0}
    rx2y0[e]{down:rx2y1,left:rx1y0}
    rx0y1[e]{down:rx0y2,right:rx1y1,up:rx0y0}
    rx1y1[e]{down:rx1y2,left:rx0y1,right:rx2y1,up:rx1y0}
    rx2y1[e]{down:rx2y2,left:rx1y1,up:rx2y0}
    rx0y2[e]{right:rx1y2,up:rx0y1}
    rx1y2[e]{left:rx0y2,right:rx2y2,up:rx1y1}
    rx2y2[e]{left:rx1y2,up:rx2y1}
#players = oplayer(100),xplayer(100)
#pieces = e,o,x
#variables =
#rules =
//1             2
  [$ xplayer=50][$ oplayer=50]
(
  (
//3
  ->xplayer (up* + down*)(left* + right*)){e}
//4   5
  ->> [x]
  (
    {!
      up left {x} (up left + down right down right){x} +
      down right {x} down right {x} +
      up {x}(up + down down) {x} +
      down {x}down {x} +
      up right {x} (up right + down left down left){x} +
      down left {x}down left {x} +
      left {x} (left + right right) {x} +
      right {x} right {x}
    } +
    {?
      up left {x} (up left + down right down right){x} +
      down right {x} down right {x} +
      up {x}(up + down down) {x} +
      down {x}down {x} +
      up right {x} (up right + down left down left){x} +
      down left {x}down left {x} +
      left {x} (left + right right) {x} +
      right {x} right {x}
    }
//  6              7             8
    [$ xplayer=100][$ oplayer=0] ->> {}
  )
//9
  ->oplayer (up* + down*)(left* + right*) {e}
//10  11
  ->> [o]
  (
  {!
    up left {o}( up left + down right down right){o} +
    down right {o}down right {o} +
    up {o}( up + down down){o} +
    down {o}down {o} +
    up right {o}(up right + down left down left){o} +
    down left {o}down left {o} +
    left {o}(left + right right){o} +
    right {o}right {o}
  }
  +
  {?
    up left {o}( up left + down right down right){o} +
    down right {o}down right {o} +
    up {o}( up + down down){o} +
    down {o}down {o} +
    up right {o}(up right + down left down left){o} +
    down left {o}down left {o} +
    left {o}(left + right right){o} +
    right {o}right {o}
  }
//12             13           14
  [$ oplayer=100][$ xplayer=0]->>{}
  )
)*

)LIM";
}

int main() {
  auto parsed_tic_tac_toe = ParseGame(kTicTacToeGame);
  auto parse_labeled = ParseGame(kLabeledTicTacToeGame);
  std::cout << "The games are the same " << (parsed_tic_tac_toe->to_rbg(rbg_parser::options{}) == parse_labeled->to_rbg(rbg_parser::options{})) << std::endl;

  auto game = CreateGameState(kLabeledTicTacToeGame);
  auto moves = game.Moves();
  std::vector<bool> exists(9);
  std::cout << RectangularBoardDescription(game.board_content(), game.declarations());
  std::cout << VariablesValuesDescription(game);
  std::cout << "Possible tic tac toe o moves after x middle move (5 4): " << std::endl;
  std::cout << "Possible tic tac toe x moves: " << std::endl;
  for(const auto& move : moves) {
    auto move_text = EncodeMove(move);
    std::cout << move_text << " | ";
    std::stringstream stream(move_text);
    int vertex, identifier;
    stream >> vertex >> identifier;
    assert(identifier == 4); // The identifier of the keeper switch
    exists[vertex-1] = true;
  }
  std::cout << std::endl;
  for(size_t i = 0; i < exists.size(); i++) {
    assert(exists[i] == true);
  }
  auto middle_move = DecodeMove("5 4");
  game.Apply(middle_move);
  moves = game.Moves();
  std::cout << RectangularBoardDescription(game.board_content(), game.declarations());
  std::cout << VariablesValuesDescription(game);
  std::cout << "Possible tic tac toe o moves after x middle move (5 4): " << std::endl;
  exists.clear();
  for(const auto& move : moves) {
    auto move_text = EncodeMove(move);
    std::cout << move_text << " | ";
    std::stringstream stream(move_text);
    int vertex, identifier;
    stream >> vertex >> identifier;
    assert(identifier == 10); // The identifier of the keeper switch
    exists[vertex-1] = true;
  }
  std::cout << std::endl;
  for(size_t i = 0; i < exists.size(); i++) {
    if (i == 4) {
      assert(exists[i] == false);
    }
    else {
      assert(exists[i] == true);
    }
  }
  auto right_upper_corner_move = DecodeMove("3 10");
  game.Apply(right_upper_corner_move);
  std::cout << RectangularBoardDescription(game.board_content(), game.declarations());
  std::cout << VariablesValuesDescription(game);
  moves = game.Moves();
  std::cout << "Possible tic tac toe x moves after x middle move (5 4) and right upper corner o move (3 10): " << std::endl;
  exists.clear();
  for(const auto& move : moves) {
    auto move_text = EncodeMove(move);
    std::cout << move_text << " | ";
    std::stringstream stream(move_text);
    int vertex, identifier;
    stream >> vertex >> identifier;
    assert(identifier == 4); // The identifier of the keeper switch
    exists[vertex-1] = true;
  }
  std::cout << std::endl;
  for(size_t i = 0; i < exists.size(); i++) {
    if (i == 4 || i == 2) {
      assert(exists[i] == false);
    }
    else {
      assert(exists[i] == true);
    }
  }
}
