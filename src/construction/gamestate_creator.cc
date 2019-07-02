//
// Created by shrum on 02.07.19.
//

#include "gamestate_creator.h"

rbg::GameState rbg::CreateGameState(const std::string &game_text) {
  auto game = ParseGame(game_text);
  Declarations declarations = CreateDeclarations(*game);
  NfaWithVisitedChecks nfa = CreateGraph(*game->get_moves(), declarations);
  SearchStepsInformation steps = CreateSearchSteps(nfa, declarations);
  return GameState(declarations, std::move(steps));
}
