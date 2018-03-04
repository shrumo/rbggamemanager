//
// Created by shrum on 12.01.18.
//

#include "action.h"
#include "../game_components/game_state.h"

ActionResult actions::Shift::Apply(GameState *b) const {
  b->current_x_ += delta_x_;
  b->current_y_ += delta_y_;
  return b->x() < b->width() && b->y() < b->height();
}

void actions::Shift::Revert(GameState *state, const ActionResult &) const {
  state->current_x_ -= delta_x_;
  state->current_y_ -= delta_y_;
}

ActionResult actions::On::Apply(GameState *b) const {
  return {pieces_[b->CurrentPiece()]};
}

ActionResult actions::Off::Apply(GameState *b) const {
  b->lazy().EvaluateLazyReversible(b);
  size_t previous_piece = b->CurrentPiece();
  b->SetPiece(piece_);
  return {true, previous_piece};
}

void
actions::Off::Revert(GameState *b, const ActionResult &apply_result) const {
  b->SetPiece(apply_result.revert_piece());
  b->lazy().RevertLastLazyEvaluation(b);
}

ActionResult actions::PlayerSwitch::Apply(GameState *b) const {
  b->lazy().EvaluateLazyReversible(b);
  size_t previous_player = b->player();
  b->current_player_ = player_;
  return {true, previous_player};
}

void actions::PlayerSwitch::Revert(GameState *b,
                                   const ActionResult &apply_result) const {
  b->current_player_ = apply_result.revert_player();
  b->lazy().RevertLastLazyEvaluation(b);
}

ActionResult actions::Assignment::Apply(GameState *b) const {
  b->lazy().EvaluateLazyReversible(b);
  int previous_value = b->Value(variable);
  b->sigma_[variable] = value->Value(b);
  return {true, previous_value};
}

void actions::Assignment::Revert(GameState *b,
                                 const ActionResult &apply_result) const {
  b->sigma_[variable] = apply_result.revert_value();
  b->lazy().RevertLastLazyEvaluation(b);
}

ActionResult actions::Lazy::Apply(GameState *b) const {
  b->lazy().AddLazyAction(b->x(), b->y(), lazy_action_);
  return true;
}

void actions::Lazy::Revert(GameState *b, const ActionResult &) const {
  b->lazy().PopLazyAction();
}

ActionResult actions::Incrementation::Apply(GameState *b) const {
  b->sigma_[variable_]++;
  b->sigma_[variable_] %= b->description().bound();
  return true;
}

void actions::Incrementation::Revert(GameState *b, const ActionResult &) const {
  b->sigma_[variable_]--;
  b->sigma_[variable_] += b->description().bound();
  b->sigma_[variable_] %= b->description().bound();
}

ActionResult actions::Decrementation::Apply(GameState *b) const {
  b->sigma_[variable_]--;
  b->sigma_[variable_] += b->description().bound();
  b->sigma_[variable_] %= b->description().bound();
  return true;
}

void actions::Decrementation::Revert(GameState *b, const ActionResult &) const {
  b->sigma_[variable_]++;
  b->sigma_[variable_] %= b->description().bound();
}
