//
// Created by shrum on 12.01.18.
//

#include "lazy_evaluator.h"
#include "game_state.h"

void lazy_evaluator::revert_last_lazy_evaluation(game_state *state) {
    if(evaluating)
        return;
    evaluating=true;
    size_t revert_point = evaluation_heads.back();
    evaluation_heads.pop_back();
    while(action_results.size() > revert_point)
    {
        state->revert_action_application(lazy_actions[action_results.size()-1], action_results.back());
        action_results.pop_back();
    }
    evaluating=false;
}

void lazy_evaluator::evaluate_lazy_reversible(game_state *state) {
    if(evaluating)
        return;
    evaluating=true;
    size_t old_lazy_head = action_results.size();
    while(action_results.size() < lazy_actions.size())
    {
        action_results.push_back(state->apply_action_application(lazy_actions[action_results.size()]));
    }
    evaluation_heads.push_back(old_lazy_head);
    evaluating=false;
}
