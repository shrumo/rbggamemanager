//
// Created by shrum on 27.12.17.
//

#ifndef RBGGAMEMANAGER_AUTOMATON_H
#define RBGGAMEMANAGER_AUTOMATON_H

#include <vector>
#include <unordered_set>
#include <memory>

namespace fsm {
    template<typename Letter>
    class state;

    using state_id_t = size_t;

    template<typename Letter>
    class transition {
        state_id_t target_id;
        std::unique_ptr<Letter> letter;
    public:
        transition(state_id_t target_id)
                : target_id(target_id), letter(nullptr)
        {
        }

        transition(state_id_t target_id, std::unique_ptr<Letter> letter)
                : target_id(target_id), letter(std::move(letter))
        {
        }

        bool is_epsilon() const
        {
            return !letter;
        }

        state_id_t target_state_id() const
        {
            return target_id;
        }

        Letter* get_letter() const
        {
            return letter.get();
        }
    };

    template<typename Letter>
    class state {
        std::vector<transition<Letter> > transitions;
    public:
        state() = default;

        const std::vector<transition<Letter> >& get_transitions() const
        {
            return transitions;
        }

        void add_transition(state_id_t target_id) {
            transitions.emplace_back(target_id);
        }

        void add_transition(state_id_t target_id,std::unique_ptr<Letter> letter) {
            transitions.emplace_back(target_id, std::move(letter));
        }
    };

    template<typename Letter>
    class state_register
    {
        std::vector<state<Letter> > states;
    public:
        state_register() = default;
        state_register(const state_register&) = delete;
        state_id_t new_state()
        {
            states.emplace_back();
            return states.size() - 1;
        }

        state<Letter>& operator[](state_id_t id)
        {
            return states[id];
        }

        const state<Letter>& operator[](state_id_t id) const
        {
            return states[id];
        }

        size_t size() const
        {
            return states.size();
        }
    };

    template<typename Letter>
    class nfa {
        state_id_t initial_state_id;
        state_id_t final_state_id;

        state_register<Letter> *states;
    public:
        nfa(state_register<Letter> *reg)
                : states(reg)
        {}

        nfa(const nfa& a) = default;

        state<Letter>& operator[](state_id_t id)
        {
            return (*states)[id];
        }

        const state<Letter>& operator[](state_id_t id) const
        {
            return (*states)[id];
        }

        state_id_t get_initial_id() const
        {
            return initial_state_id;
        }

        state_id_t get_final_id() const
        {
            return final_state_id;
        }

        state_id_t new_state() {
            return states->new_state();
        }

        state<Letter>& initial()
        {
            return (*states)[initial_state_id];
        }

        state<Letter>& final()
        {
            return (*states)[final_state_id];
        }

        void set_initial(state_id_t state_id)
        {
            initial_state_id = state_id;
        }

        void set_final(state_id_t state_id)
        {
            final_state_id = state_id;
        }
    };
}






#endif //RBGGAMEMANAGER_AUTOMATON_H
