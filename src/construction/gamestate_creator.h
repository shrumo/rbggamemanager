//
// Created by shrum on 28.06.19.
//

#ifndef RBGGAMEMANAGER_GAMESTATE_CREATOR_H
#define RBGGAMEMANAGER_GAMESTATE_CREATOR_H

#include <game_state/game_state.h>
#include "declarations_creator.h"
#include <parser/parser_wrapper.h>

namespace rbg {
  GameState CreateGameState(const std::string &game_text) {
    auto game = ParseGame(game_text);
    Declarations declarations = CreateDeclarations(*game);
    NfaWithVisitedChecks nfa = CreateGraph(*game->get_moves(), declarations);
    SearchStepsCollection steps = CreateSearchSteps(nfa, declarations);
    return GameState(declarations, std::move(steps));
  }
}

#endif //RBGGAMEMANAGER_GAMESTATE_CREATOR_H
