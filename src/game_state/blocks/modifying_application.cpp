//
// Created by shrum on 18/07/19.
//

#include "modifying_application.h"
#include <game_state/game_state.h>

using namespace rbg;

void ModifyingApplication::PushVisitedStackAndAddToApplied(rbg::GameState *state) const {
  state->steps_.collection.PushStacks();
  state->applied_modifiers_.push_back(ModifierApplication{state->current_pos_, index_});
}

void ModifyingApplication::PopVisitedStackAndAddToApplied(rbg::GameState *state)  const {
  state->applied_modifiers_.pop_back();
  state->steps_.collection.PopStacks();
}

vertex_id_t ModifyingApplication::weird_current_state_pos(GameState *state) const {
  return state->current_pos_;
}

void ModifyingApplication::RegisterMove(GameState *state) const {
  state->moves_.push_back(state->applied_modifiers_);
}
