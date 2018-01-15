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

    std::size_t evaluate(game_state* state);
    void revert(game_state* state, std::size_t revert_point);
};


#endif //RBGGAMEMANAGER_LAZY_EVALUATOR_H
