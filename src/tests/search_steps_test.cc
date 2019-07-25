//
// Created by shrum on 28.06.19.
//

#include <game_state/construction/gamestate_creator.h>
#include <iostream>

using namespace rbg;
using namespace std;

const string kSmallGameDeclarations = R"LIM(
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r]
    [e,e])

//   Board indices:
//   0 1
//   2 3
)LIM";

void TestPlayerSwitch() {
  auto game = CreateGameState(kSmallGameDeclarations + "#rules = ->red ->blue");
  auto moves = game.Moves();
  assert(moves.size() == 1);
  game.Apply(moves[0]);
  assert(game.current_player() == game.declarations().players_resolver.Id("blue"));
}

void TestMultipleSearchStep() {
  auto game = CreateGameState(kSmallGameDeclarations + "#rules = ->red (->blue + ->red)");
  auto moves = game.Moves();
  assert(moves.size() == 2);
}

void TestShiftCheckSearchStep() {
  auto game = CreateGameState(kSmallGameDeclarations + "#rules = ->red right ->blue");
  auto moves = game.Moves();
  assert(moves.size() == 1);
  assert(moves[0][0].vertex == 1);
  auto game_no_move = CreateGameState(kSmallGameDeclarations + "#rules = ->red right right ->blue");
  auto moves_empty = game_no_move.Moves();
  assert(moves_empty.empty());
}

void TestOnCheckSearchStep() {
  auto game = CreateGameState(kSmallGameDeclarations + "#rules = ->red {b} ->blue");
  auto moves = game.Moves();
  assert(moves.size() == 1);
  auto game_no_move = CreateGameState(kSmallGameDeclarations + "#rules = ->red {e} ->blue");
  auto moves_empty = game_no_move.Moves();
  assert(moves_empty.empty());
}

void TestVisitedCheckSearchStep() {
  auto game = CreateGameState(kSmallGameDeclarations + "#rules = ->red (right {r} left + {b}) ->blue");
  auto moves = game.Moves();
  assert(moves.size() == 1);
}

void TestAssignment() {
  auto game = CreateGameState(kSmallGameDeclarations + "#rules = ->red [$turn = 10] [$blue = 15] ->blue");
  auto moves = game.Moves();
  assert(moves.size() == 1);
  assert(moves[0].size() == 3);
  game.Apply(moves[0]);
  assert(game.variables_values()[game.declarations().variables_resolver.Id("turn")] == 10);
  assert(game.variables_values()[game.declarations().variables_resolver.Id("blue")] == 15);
  auto game_no_move = CreateGameState(
      kSmallGameDeclarations + "#rules = ->red [$turn = 10] [$blue = turn + 70] ->blue");
  auto moves_empty = game_no_move.Moves();
  assert(moves_empty.empty());
}

void TestExpressions() {
  auto game = CreateGameState(
      kSmallGameDeclarations + "#rules = ->red [$blue = 6] [$red = ((blue + 2) * 4 + 1) / (5 - 1) ] ->blue");
  auto moves = game.Moves();
  assert(moves.size() == 1);
  assert(moves[0].size() == 3);
  game.Apply(moves[0]);
  assert(game.variables_values()[game.declarations().variables_resolver.Id("blue")] == 6);
  assert(game.variables_values()[game.declarations().variables_resolver.Id("red")] == ((6 + 2) * 4 + 1) / (5 - 1) &&
         "Make sure the division is an integer divison.");
}

void TestLessComparison() {
  auto game = CreateGameState(
      kSmallGameDeclarations + "#rules = ->red [$blue = 6] [$turn = 7] {$ blue < turn + 2} ->blue");
  auto moves = game.Moves();
  assert(moves.size() == 1);
  auto game_no_move = CreateGameState(
      kSmallGameDeclarations + "#rules = ->red [$blue = 6] [$turn = 7] {$ blue < turn / 2} ->blue");
  auto moves_empty = game_no_move.Moves();
  assert(moves_empty.empty());
}

void TestLessEqualComparison() {
  auto game = CreateGameState(
      kSmallGameDeclarations + "#rules = ->red [$blue = 6] [$turn = 7] {$ blue <= turn - 1} ->blue");
  auto moves = game.Moves();
  assert(moves.size() == 1);
  auto game_no_move = CreateGameState(
      kSmallGameDeclarations + "#rules = ->red [$blue = 6] [$turn = 7] {$ blue <= turn / 2} ->blue");
  auto moves_empty = game_no_move.Moves();
  assert(moves_empty.empty());
}

void TestEqualComparison() {
  auto game = CreateGameState(
      kSmallGameDeclarations + "#rules = ->red [$blue = 6] [$turn = 7] {$ blue == turn -1} ->blue");
  auto moves = game.Moves();
  assert(moves.size() == 1);
  auto game_no_move = CreateGameState(
      kSmallGameDeclarations + "#rules = ->red [$blue = 6] [$turn = 7] {$ blue == turn} ->blue");
  auto moves_empty = game_no_move.Moves();
  assert(moves_empty.empty());
}


void TestNotEqualComparison() {
  auto game = CreateGameState(
      kSmallGameDeclarations + "#rules = ->red [$blue = 6] [$turn = 7] {$ blue != turn - 7} ->blue");
  auto moves = game.Moves();
  assert(moves.size() == 1);
  auto game_no_move = CreateGameState(
      kSmallGameDeclarations + "#rules = ->red [$blue = 6] [$turn = 7] {$ blue != turn - 1} ->blue");
  auto moves_empty = game_no_move.Moves();
  assert(moves_empty.empty());
}

void TestConditionCheckStep() {
  auto game = CreateGameState(kSmallGameDeclarations + "#rules = ->red {? right [e] (left + down)* {e} }->blue");
  auto moves = game.Moves();
  assert(moves.size() == 1);
  auto game_no_move = CreateGameState(
      kSmallGameDeclarations + "#rules = ->red  {? right [e] (left + down)* {e} up {r}} ->blue");
  auto moves_empty = game_no_move.Moves();
  assert(moves_empty.empty());
}


int main() {
  try {
    TestPlayerSwitch();
    TestMultipleSearchStep();
    TestShiftCheckSearchStep();
    TestOnCheckSearchStep();
    TestVisitedCheckSearchStep();
    TestAssignment();
    TestExpressions();
    TestLessComparison();
    TestLessEqualComparison();
    TestEqualComparison();
    TestNotEqualComparison();
    TestConditionCheckStep();
  } catch (const rbg_parser::message &m) {
    std::cout << m.as_error() << std::endl;
  }
}