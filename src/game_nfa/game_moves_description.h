//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_GAME_AUTOMATON_H
#define RBGGAMEMANAGER_GAME_AUTOMATON_H

#include <memory>
#include "automaton.h"
#include "../actions/action.h"

class game_moves_description
{
    std::unique_ptr<fsm::nfa<const action*>> nfa;
    std::vector<fsm::state_id_t> blocks_states;
    std::vector<std::unique_ptr<action> > actions;
    unsigned int move_pattern_count;
    game_moves_description(std::unique_ptr<fsm::nfa<const action *>> nfa,
                           std::vector<fsm::state_id_t> blocks_states,
                           std::vector<std::unique_ptr<action>> actions, unsigned int move_pattern_count) : nfa(
            std::move(nfa)), blocks_states(std::move(blocks_states)), actions(std::move(actions)), move_pattern_count(move_pattern_count)
    {}
public:
    const fsm::nfa<const action*>& get_nfa() const
    {
        return *nfa;
    }

    fsm::state_id_t get_corresponding_state(size_t block_id) const
    {
        return blocks_states[block_id];
    }

    unsigned int get_move_pattern_count() const
    {
        return move_pattern_count;
    }

    friend class game_nfa_creator;
};



#endif //RBGGAMEMANAGER_GAME_AUTOMATON_H
