//
// Created by shrum on 12.01.18.
//

#include "lazy_evaluator.h"
#include "game_state.h"

void LazyEvaluator::RevertLastLazyEvaluation(GameState *state) {
  if (evaluating_)
    return;
  evaluating_ = true;
  size_t revert_point = evaluation_heads_.back();
  evaluation_heads_.pop_back();
  while (action_results_.size() > revert_point) {
    state->RevertActionApplication(lazy_actions_[action_results_.size() - 1],
                                   action_results_.back());
    action_results_.pop_back();
  }
  evaluating_ = false;
}

void LazyEvaluator::EvaluateLazyReversible(GameState *state) {
  if (evaluating_)
    return;
  evaluating_ = true;
  size_t old_lazy_head = action_results_.size();
  while (action_results_.size() < lazy_actions_.size()) {
    action_results_.push_back(
        state->ApplyActionApplication(lazy_actions_[action_results_.size()]));
  }
  evaluation_heads_.push_back(old_lazy_head);
  evaluating_ = false;
}
