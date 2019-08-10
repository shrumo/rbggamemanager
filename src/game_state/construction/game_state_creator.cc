//
// Created by shrum on 02.07.19.
//

#include "game_state_creator.h"

rbg::GameState rbg::CreateGameState(const std::string &game_text) {
  auto game = ParseGame(game_text);
  Declarations declarations = Declarations(*game);
  auto nfa = CreateNfa(*game->get_moves(), declarations);
  BlocksCollection steps = CreateSearchSteps(nfa, declarations);
  return GameState(declarations, std::move(steps));
}
