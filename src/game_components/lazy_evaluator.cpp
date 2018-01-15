//
// Created by shrum on 12.01.18.
//

#include "lazy_evaluator.h"
#include "game_state.h"

std::size_t lazy_evaluator::evaluate(game_state *state) {
    size_t old_lazy_head = action_results.size();
    while(action_results.size() < lazy_actions.size())
    {
        action_results.push_back(state->apply_action_application(lazy_actions[action_results.size()]));
    }
    return old_lazy_head;
}

void lazy_evaluator::revert(game_state *state, std::size_t revert_point) {
    while(action_results.size() > revert_point)
    {
        state->revert_action_application(lazy_actions[action_results.size()-1], action_results.back());
        action_results.pop_back();
    }
}
