//
// Created by shrum on 27.12.17.
//

#ifndef RBGGAMEMANAGER_AUTOMATON_H
#define RBGGAMEMANAGER_AUTOMATON_H

#include <vector>
#include <unordered_set>
#include <memory>
#include "rbg2gdl/src/abstract_dispatcher.hpp"

#include "action.h"
class name_resolver;

class game_move;

namespace fsm {
    class transition {
        int target_id;
        bool epsilon;
        std::unique_ptr<action> move;
    public:
        transition(int target_id)
                : target_id(target_id), epsilon(true)
        {
        }

        transition(int target_id, action* ptr)
                : target_id(target_id), epsilon(false), move(ptr)
        {
        }

        bool is_epsilon() const
        {
            return epsilon;
        }

        int target_state_id() const
        {
            return target_id;
        }

        action* get_action() const
        {
            return move.get();
        }
    };

    class state {
        int id;
        std::vector<transition> transitions;
    public:
        state(int id)
                : id(id) {}

        const std::vector<transition>& get_transitions() const
        {
            return transitions;
        }

        void add_transition(int target_id) {
            transitions.emplace_back(target_id);
        }

        void add_transition(int target_id, action *move) {
            transitions.emplace_back(target_id, move);
        }
    };

    class state_register
    {
    public:
        std::vector<state> states;

        int new_state()
        {
            states.emplace_back(states.size());
            return (int) (states.size() - 1);
        }
    };

    class nfa {
        int initial_state_id;
        int final_state_id;

        state_register* states;
    public:
        nfa(state_register* reg) : initial_state_id(-1), final_state_id(-1), states(reg)
        {}

        nfa(const nfa& a) = default;

        state& operator[](int id)
        {
            return states->states[id];
        }

        const state& operator[](int id) const
        {
            return states->states[id];
        }

        int get_initial_id() const
        {
            return initial_state_id;
        }

        int get_final_id() const
        {
            return final_state_id;
        }

        state &initial_state() {
            return states->states[initial_state_id];
        }

        state &final_state() {
            return states->states[final_state_id];
        }

        int new_state() {
            return states->new_state();
        }

        void set_initial(int state_id)
        {
            initial_state_id = state_id;
        }

        void set_final(int state_id)
        {
            final_state_id = state_id;
        }

        state_register* get_state_register() const
        {
            return states;
        }
    };
}

class game_nfa_creator : public rbg_parser::abstract_dispatcher{
    int counter;
    fsm::nfa result;
    const name_resolver& resolver;
public:

    game_nfa_creator(fsm::state_register* ref, const name_resolver& resolver)
        :result(ref), counter(0), resolver(resolver)
    {
    }

    game_nfa_creator(const game_nfa_creator& parent)
            :result(parent.get_result().get_state_register()), counter(parent.counter), resolver(parent.resolver)
    {
    }

    const fsm::nfa& get_result() const
    {
        return result;
    }

    void dispatch(const rbg_parser::sum&) override;
    void dispatch(const rbg_parser::pure_sum&) override;
    void dispatch(const rbg_parser::concatenation&) override;
    void dispatch(const rbg_parser::pure_concatenation&) override;
    void dispatch(const rbg_parser::bracketed_move&) override;
    void dispatch(const rbg_parser::pure_bracketed_move&) override;
    void dispatch(const rbg_parser::shift&) override;
    void dispatch(const rbg_parser::ons&) override;
    void dispatch(const rbg_parser::off&) override;
    void dispatch(const rbg_parser::assignment&) override;
    void dispatch(const rbg_parser::player_switch&) override;
    void dispatch(const rbg_parser::condition_check&) override;
    void dispatch(const rbg_parser::conjunction&) override;
    void dispatch(const rbg_parser::alternative&) override;
    void dispatch(const rbg_parser::negatable_condition&) override;
    void dispatch(const rbg_parser::comparison&) override;
    void dispatch(const rbg_parser::move_condition&) override;
};






#endif //RBGGAMEMANAGER_AUTOMATON_H
