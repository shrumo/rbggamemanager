//
// Created by shrum on 28.06.19.
//

#ifndef RBGGAMEMANAGER_GAME_STATE_CREATOR_H
#define RBGGAMEMANAGER_GAME_STATE_CREATOR_H

#include <game_state/game_state.h>
#include <parser/parser_wrapper.h>

namespace rbg {
  GameState CreateGameState(const std::string &game_text);
}

#endif //RBGGAMEMANAGER_GAME_STATE_CREATOR_H
