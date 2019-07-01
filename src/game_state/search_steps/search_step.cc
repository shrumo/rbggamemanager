//
// Created by shrum on 27.06.19.
//

#include "search_step.h"
#include <game_state/game_state.h>

using namespace rbg;

bool VisitedCheckSearchStep::Check(GameState &state) {
  bool result = stack_chunk_[state.current_pos_];
  stack_chunk_.set(state.current_pos_);
  return !result;
}

bool ShiftStep::SetAndCheck(GameState &state, vertex_id_t &previous_vertex) {
  previous_vertex = state.current_pos_;
  state.current_pos_ = state.declarations().board_description.NextVertex(state.current_pos_, edge_id_);
  return state.current_pos_ != state.board_.vertices_count();
}

void ShiftStep::Revert(GameState &state, vertex_id_t previous_vertex) {
  state.current_pos_ = previous_vertex;
}

bool OnStep::Check(GameState &state) {
  return pieces_[state.board_.at(state.current_pos_)];
}

bool ArithmeticLessEqualComparison::Check(GameState &state) {
  return left_->Value(state) <= right_->Value(state);
}

bool ArithmeticLessComparison::Check(GameState &state) {
  return left_->Value(state) < right_->Value(state);
}

bool ArithmeticEqualComparison::Check(GameState &state) {
  return left_->Value(state) == right_->Value(state);
}

bool ArithmeticNotEqualComparison::Check(GameState &state) {
  return left_->Value(state) != right_->Value(state);
}

bool ConditionCheckStep::Check(GameState &state) {
  steps_collection_->stack().reset();
  return steps_collection_->current()->EndStepReachable(state);
}

ConditionCheckStep::ConditionCheckStep(std::unique_ptr<SearchStepsCollection> steps_collection)
    : steps_collection_(std::move(steps_collection)) {}

bool NegatedConditionCheckStep::Check(GameState &state) {
  steps_collection_->stack().reset();
  return !steps_collection_->current()->EndStepReachable(state);
}

NegatedConditionCheckStep::NegatedConditionCheckStep(std::unique_ptr<SearchStepsCollection> steps_collection)
    : steps_collection_(std::move(steps_collection)) {}

void OffStep::Set(GameState &state, piece_id_t &previous_piece) const {
  previous_piece = state.board_.at(state.current_pos_);
  state.board_.set(state.current_pos_, piece_id_);
}

void OffStep::Reset(GameState &state, piece_id_t previous_piece) const {
  state.board_.set(state.current_pos_, previous_piece);
}

void OffStep::Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
                  std::vector<std::vector<ModifierApplication>> &moves) {
  piece_id_t revert_info;
  Set(state, revert_info);
  applied_modifiers.push_back({state.current_pos_, index()});
  state.steps_.stack().Push();
  RunNextStep(state, applied_modifiers, moves);
  state.steps_.stack().Pop();
  applied_modifiers.pop_back();
  Reset(state, revert_info);
}

bool OffStep::ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                              vertex_id_t revert_vertex) {
  piece_id_t previous_piece;
  Set(state, previous_piece);
  reverting_information.push_back({*this, revert_vertex, previous_piece});
  if (ApplyNextStepUntilFound(state, reverting_information, state.current_pos_)) {
    return true;
  }
  reverting_information.pop_back();
  Reset(state, previous_piece);
  return false;
}

bool OffStep::EndStepReachable(GameState &state) {
  piece_id_t previous_piece;
  Set(state, previous_piece);
  bool result = EndStepReachableFromNext(state);
  Reset(state, previous_piece);
  return result;
}

StepRevertInformation OffStep::ApplyReversibleAtVertex(GameState &state, vertex_id_t vertex) const {
  vertex_id_t previous_vertex = state.current_pos_;
  state.current_pos_ = vertex;
  piece_id_t previous_piece;
  Set(state, previous_piece);
  return {*this, previous_vertex, previous_piece};
}

void OffStep::RevertFromRevertInfo(GameState &state, const StepRevertInformation &revert_info) const {
  Reset(state, revert_info.revert_info);
  state.current_pos_ = revert_info.vertex;
}

void PlayerSwitchStep::Set(GameState &state, player_id_t &previous_player) const {
  previous_player = state.current_player_;
  state.current_player_ = player_id_;
}

void PlayerSwitchStep::Reset(GameState &state, player_id_t previous_player) const {
  state.current_player_ = previous_player;
}

void PlayerSwitchStep::Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
                           std::vector<std::vector<ModifierApplication>> &moves) {
  player_id_t revert_info;
  Set(state, revert_info);
  applied_modifiers.push_back({state.current_pos_, index()});
  moves.push_back(applied_modifiers);
  applied_modifiers.pop_back();
  Reset(state, revert_info);
}

bool PlayerSwitchStep::ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                                       vertex_id_t revert_vertex) {
  player_id_t previous_player;
  Set(state, previous_player);
  reverting_information.push_back({*this, revert_vertex, previous_player});
  return true;
}

bool PlayerSwitchStep::EndStepReachable(GameState &state) {
  player_id_t previous_player;
  Set(state, previous_player);
  return EndStepReachableFromNext(state);
}

StepRevertInformation PlayerSwitchStep::ApplyReversibleAtVertex(GameState &state, vertex_id_t vertex) const {
  vertex_id_t previous_vertex = state.current_pos_;
  state.current_pos_ = vertex;
  player_id_t revert_info;
  Set(state, revert_info);
  return {*this, previous_vertex, revert_info};
}

void PlayerSwitchStep::RevertFromRevertInfo(GameState &state, const StepRevertInformation &info) const {
  state.current_pos_ = info.vertex;
  Reset(state, info.revert_info);
}

bool AssignmentStep::SetAndCheck(GameState &state, variable_value_t &previous_value) const {
  previous_value = state.variables_values_[variable_id_];
  state.variables_values_[variable_id_] = value_->Value(state);
  return state.variables_values_[variable_id_] <= state.declarations().variables_bounds[variable_id_];
}

void AssignmentStep::Reset(GameState &state, variable_value_t previous_value) const {
  state.variables_values_[variable_id_] = previous_value;
}

void AssignmentStep::Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
                         std::vector<std::vector<ModifierApplication>> &moves) {
  variable_value_t previous_value;
  if (SetAndCheck(state, previous_value)) {
    applied_modifiers.push_back({state.current_pos_, index()});
  state.steps_.stack().Push();
  RunNextStep(state, applied_modifiers, moves);
  state.steps_.stack().Pop();
  applied_modifiers.pop_back();
  }
  Reset(state, previous_value);
}

bool AssignmentStep::ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                                     vertex_id_t revert_vertex) {
  variable_value_t previous_value;
  if (SetAndCheck(state, previous_value)) {
    reverting_information.push_back({*this, revert_vertex, previous_value});
    if (ApplyNextStepUntilFound(state, reverting_information, state.current_pos_)) {
      return true;
    }
    reverting_information.pop_back();
  }
  Reset(state, previous_value);
  return false;
}

bool AssignmentStep::EndStepReachable(GameState &state) {
  variable_value_t previous_value;
  bool result = false;
  if (SetAndCheck(state, previous_value)) {
    result = EndStepReachableFromNext(state);
  }
  Reset(state, previous_value);
  return result;
}

StepRevertInformation AssignmentStep::ApplyReversibleAtVertex(GameState &state, vertex_id_t vertex) const {
  vertex_id_t previous_vertex = state.current_pos_;
  state.current_pos_ = vertex;
  variable_value_t previous_value;
  SetAndCheck(state, previous_value);
  return {*this, previous_vertex, previous_value};
}

void AssignmentStep::RevertFromRevertInfo(GameState &state, const StepRevertInformation &info) const {
  state.current_pos_ = info.vertex;
  Reset(state, info.revert_info);
}

bool PlayerCheckStep::Check(GameState &state) {
  return state.current_player_ == player_;
}
