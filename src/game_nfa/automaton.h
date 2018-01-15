//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_AUTOMATON_H
#define RBGGAMEMANAGER_AUTOMATON_H

#include<vector>

namespace fsm {
    using state_id_t = std::size_t;

    template<typename Letter>
    class transition
    {
        state_id_t target_id;
        Letter transition_letter;
    public:
        transition(state_id_t target_id, const Letter& letter)
                : target_id(target_id), transition_letter(letter) {}

        state_id_t target() const
        {
            return target_id;
        }
        const Letter& letter() const
        {
            return transition_letter;
        }
    };

    template <typename Letter>
    class state
    {
        std::vector<transition<Letter> > state_transitions;
    public:
        state() = default;
        const std::vector<transition<Letter> >& transitions() const
        {
            return state_transitions;
        }

        void add_transition(state_id_t target_id, const Letter& letter)
        {
            state_transitions.emplace_back(target_id, letter);
        }
    };

    template <typename Letter>
    class nfa
    {
        state_id_t initial_state_id;
        state_id_t final_state_id;
        std::vector<state<Letter> > states;
    public:
        nfa()
            : initial_state_id(0),
              final_state_id(0) {}

        state<Letter>& operator[](state_id_t id)
        {
            return states[id];
        }

        const state<Letter>& operator[](state_id_t id) const
        {
            return states[id];
        }

        state_id_t initial() const
        {
            return initial_state_id;
        }

        state_id_t final() const
        {
            return final_state_id;
        }

        state_id_t new_state()
        {
            states.emplace_back();
            return states.size() - 1;
        }

        void set_initial(state_id_t id)
        {
            initial_state_id = id;
        }

        void set_final(state_id_t id)
        {
            final_state_id = id;
        }

        std::size_t get_state_count() const
        {
            return states.size();
        }
    };
}


#endif //RBGGAMEMANAGER_AUTOMATON_H
