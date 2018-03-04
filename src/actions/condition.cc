//
// Created by shrum on 12.01.18.
//

#include "condition.h"
#include "../game_components/game_state.h"

bool conditions::MovePattern::Check(GameState *b) const {
  return b->search_context().CheckPattern(*move_nfa_, index_);
}
