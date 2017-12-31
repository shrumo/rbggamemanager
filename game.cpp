//
// Created by shrum on 28.12.17.
//

#include "game.h"

std::vector<move> find_all_moves(game_state* current) {
    std::vector<move> result;
    move current_move;
    std::unordered_set<long long> visited;
    find_all_moves_rec(current, &current_move, &result,&visited);
    return result;
}

void find_all_moves_rec(game_state *state, move *current_move, std::vector<move> *result,
                        std::unordered_set<long long>* visited) {
    if(visited->find(state->small_identifier()) != visited->end())
        return;
    visited->insert(state->small_identifier());
    for(const fsm::transition& x : state->parent.game_nfa[state->current_nfa_state].get_transitions())
    {
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
            if(x.get_action()->apply(state)) {
                if(x.get_action()->is_modifier())
                {
                    current_move->move_actions.emplace_back(state->board_x,state->board_y,x.get_action()->get_index());
                }
                if (x.get_action()->is_switch()) {
                    result->emplace_back(*current_move);

                    x.get_action()->revert(state);
                    state->current_nfa_state = previous_state;
                    return;
                }
                state->current_nfa_state = x.target_state_id();
                find_all_moves_rec(state, current_move, result, visited);

                if (x.get_action()->is_modifier()) {
                    current_move->move_actions.pop_back();
                }
            }

            x.get_action()->revert(state);
            state->current_nfa_state = previous_state;
        }
    }
}

bool make_move_rec(game_state *state, move* move,std::unordered_set<long long>* visited)
{
    if(move->move_actions.empty())
        return true;
    if(visited->find(state->small_identifier()) != visited->end())
        return false;
    visited->insert(state->small_identifier());
    for(const fsm::transition& x : state->parent.game_nfa[state->current_nfa_state].get_transitions())
    {
        bool result = false;
        if(x.is_epsilon())
        {
            int previous_state = state->current_nfa_state;
            state->current_nfa_state = x.target_state_id();
            result = make_move_rec(state, move, visited);
            if(result)
                return true;
            state->current_nfa_state = previous_state;
        }
        else
        {

            int previous_state = state->current_nfa_state;
            if(x.get_action()->apply(state)) {
                bool is_ok = true;
                if(x.get_action()->is_modifier())
                {
                    auto& a = move->move_actions.front();
                    if(a.board_position_x != state->board_x || a.board_position_y != state->board_y || a.modifier_index != x.get_action()->get_index())
                    {
                        is_ok = false;
                    }
                    else
                    {
                        move->move_actions = std::vector<modifier_action>(move->move_actions.begin() + 1, move->move_actions.end());
                    }
                }
                if (x.get_action()->is_switch()) {
                    if(!move->move_actions.empty())
                        is_ok = false;
                }
                if(is_ok) {
                    state->current_nfa_state = x.target_state_id();
                    result = make_move_rec(state, move, visited);

                    if(result)
                    {
                        return true;
                    }
                    if (x.get_action()->is_modifier()) {
                        move->move_actions.emplace_back(state->board_x, state->board_y, x.get_action()->get_index());
                    }
                }
            }

            x.get_action()->revert(state);
            state->current_nfa_state = previous_state;
        }
    }
    return false;
}

void make_move(game_state *state, move move) {
    std::unordered_set<long long> visited;
    make_move_rec(state,&move,&visited);
}


