//
// Created by shrum on 28.12.17.
//

#include "game.h"

std::vector<move> find_all_moves(game_state current) {

}

void find_all_moves_rec(game_state *state, move *current_move, std::vector<move> *result,
                        const std::unordered_set<long long> visited) {
    if(visited.find(state->small_identifier()) != visited.end())
        return;
    for(const fsm::transition& x : state->parent.game_nfa[state->current_nfa_state].get_transitions())
    {
        /* TODO(shrum): if switch then add to result*/
        /* TODO(shrum): if modifier then append to current move */
        /* Maybe also refactor the code here. (One should be able to apply the transition to the state. Maybe.) */
        /* And change the identifiers of visited states to more complicated ones that take into account applied modifiers. */

        if(x.is_epsilon())
        {
            int previous_state = state->current_nfa_state;
            state->current_nfa_state = x.target_state_id();
            find_all_moves_rec(state, current_move, result, visited);
            state->current_nfa_state = previous_state;
        }
        else
        {
            int previous_state = state->current_nfa_state;
            state->current_nfa_state = x.target_state_id();
            x.get_action()->apply(state);
            find_all_moves_rec(state, current_move, result, visited);
            x.get_action()->revert(state);
            state->current_nfa_state = previous_state;
        }
    }
}
