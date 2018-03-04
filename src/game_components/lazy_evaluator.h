//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_LAZY_EVALUATOR_H
#define RBGGAMEMANAGER_LAZY_EVALUATOR_H

#include <vector>
#include "../actions/action.h"

class ActionApplication;

class GameState;

class LazyEvaluator {
public:
  LazyEvaluator()
      : evaluating_(false) {}

  template<typename ...Args>
  void AddLazyAction(Args &&... args) {
    lazy_actions_.emplace_back(std::forward<Args>(args)...);
  }

  void PopLazyAction() {
    lazy_actions_.pop_back();
  }

  void Clear() {
    lazy_actions_.clear();
    action_results_.clear();
  }

  void EvaluateLazyReversible(GameState *state);

  void RevertLastLazyEvaluation(GameState *state);

private:
  std::vector<ActionApplication> lazy_actions_;
  std::vector<ActionResult> action_results_;
  std::vector<size_t> evaluation_heads_;
  bool evaluating_;
};


#endif //RBGGAMEMANAGER_LAZY_EVALUATOR_H
