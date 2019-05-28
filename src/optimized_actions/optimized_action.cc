//
// Created by shrum on 12.01.18.
//

#include "optimized_action.h"
#include <game_components/game_state.h>

ActionResult optimizedactions::Shift::Apply(GameState *b) const {
  vertex_t previous_pos = b->current_pos_;
  b->current_pos_ = b->current_board_.Next(b->current_pos_,edge_name_);
  return {b->current_pos_ >= 0, previous_pos};
}

void optimizedactions::Shift::Revert(GameState *state, const ActionResult &r) const {
  state->current_pos_ = r.revert_pos();
}

ActionResult optimizedactions::On::Apply(GameState *b) const {
  return {pieces_[b->CurrentPiece()]};
}

ActionResult optimizedactions::Off::Apply(GameState *b) const {
  size_t previous_piece = b->CurrentPiece();
  b->SetPiece(piece_);
  return {true, previous_piece};
}

void
optimizedactions::Off::Revert(GameState *b, const ActionResult &apply_result) const {
  b->SetPiece(apply_result.revert_piece());
}

ActionResult optimizedactions::PlayerSwitch::Apply(GameState *b) const {
  size_t previous_player = b->player();
  b->current_player_ = player_;
  return {true, previous_player};
}

void optimizedactions::PlayerSwitch::Revert(GameState *b,
                                            const ActionResult &apply_result) const {
  b->current_player_ = apply_result.revert_player();
}

ActionResult optimizedactions::Assignment::Apply(GameState *b) const {
  int previous_value = b->Value(variable);
  b->sigma_[variable] = value->Value(b);
  return {0 <= b->sigma_[variable] && b->sigma_[variable] <= b->description().declarations().bound(variable), previous_value};
}

void optimizedactions::Assignment::Revert(GameState *b,
                                          const ActionResult &apply_result) const {
  b->sigma_[variable] = apply_result.revert_value();
}

ActionResult optimizedactions::ConditionCheck::Apply(GameState *b) const {
  return b->search_context().CheckPattern(*move_nfa_);
}

ActionResult optimizedactions::NegatedConditionCheck::Apply(GameState *b) const {
  return !b->search_context().CheckPattern(*move_nfa_);
}

ActionResult optimizedactions::ArithmeticLessComparison::Apply(GameState *b) const {
  return left_->Value(b) < right_->Value(b);
}

ActionResult optimizedactions::ArithmeticEqualComparison::Apply(GameState *b) const {
  return left_->Value(b) == right_->Value(b);
}

ActionResult optimizedactions::ArithmeticLessEqualComparison::Apply(GameState *b) const {
  return left_->Value(b) <= right_->Value(b);
}

ActionResult optimizedactions::ArithmeticNotEqualComparison::Apply(GameState *b) const {
  return left_->Value(b) != right_->Value(b);
}

ActionResult optimizedactions::PlayerCheck::Apply(GameState *b) const {
  return b->player() == player_;
}
