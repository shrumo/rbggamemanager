//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_LAZY_EVALUATOR_H
#define RBGGAMEMANAGER_LAZY_EVALUATOR_H

#include <vector>
#include "../actions/action.h"

class action_application;
class game_state;

class lazy_evaluator {
    std::vector<action_application> lazy_actions;
    std::vector<action_result> action_results;
    std::vector<size_t> evaluation_heads;
public:
    lazy_evaluator() = default;

    template<typename ...Args>
    void add_lazy_action(Args&&... args)
    {
        lazy_actions.emplace_back(std::forward<Args>(args)...);
    }

    void pop_lazy_action()
    {
        lazy_actions.pop_back();
    }

    void clear()
    {
        lazy_actions.clear();
        action_results.clear();
    }

    void evaluate_reversible(game_state* state);
    void revert_last_evaluation(game_state *state);

    void evaluate(game_state* state);
};


#endif //RBGGAMEMANAGER_LAZY_EVALUATOR_H
