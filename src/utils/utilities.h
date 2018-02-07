//
// Created by shrum on 06/02/18.
//

#ifndef RBGGAMEMANAGER_UTILITIES_H
#define RBGGAMEMANAGER_UTILITIES_H

#include "../actions/action.h"
#include "../game_components/game_description.h"

std::vector<const action*> get_actions_in_block(const game_moves_description& gd,size_t block_id )
{
    std::vector<const action*> result;
    fsm::state_id_t current = gd.get_corresponding_state(block_id);
    while(gd.get_nfa()[current].transitions().size() == 1 &&
          gd.get_nfa()[current].transitions().front().letter()->get_index() == block_id)
    {
        result.push_back(gd.get_nfa()[current].transitions().front().letter());
        current = gd.get_nfa()[current].transitions().front().target();
    }
    return result;
}

#endif //RBGGAMEMANAGER_UTILITIES_H
