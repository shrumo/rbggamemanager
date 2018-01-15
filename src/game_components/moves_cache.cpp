//
// Created by shrum on 14.01.18.
//

#include "moves_cache.h"
#include "game_state.h"
#include <algorithm>



moves_cache::moves_cache()
    :last_results_array_index(0), last_visited_array_index(0)
{

}

void moves_cache::find_all_moves_rec(game_state *state, size_t visited_array_index, const fsm::nfa<action *> &nfa,
                                           fsm::state_id_t current_state, move *move, bool block_started)
{
    size_t index = visited_index(state, nfa, current_state);
    size_t depth = move->get_blocks().size();
    if(visited[visited_array_index][depth][index])
        return;
    visited[visited_array_index][depth][index] = true;
    for(const auto& transition : nfa[current_state].transitions())
    {
        action_result result = transition.letter()->apply(state);
        if(result)
        {
            if(transition.letter()->is_modifier())
            {
                size_t lazy_head = state->lazy().evaluate(state);
                if(!block_started) {
                    move->add_block(state->x(), state->y(), transition.letter()->get_index());
                    create_visited_layers(visited_array_index, depth+1);
                }
                if(transition.letter()->is_switch())
                    possible_moves.push_back(*move);
                else
                {
                    find_all_moves_rec(state, visited_array_index, nfa, transition.target(), move, true);
                }
                if(!block_started) {
                    visited[visited_array_index].pop_back();
                    move->pop_block();
                }
                state->lazy().revert(state, lazy_head);
            }
            else
                find_all_moves_rec(state, visited_array_index, nfa, transition.target(), move);
        }
        transition.letter()->revert(state, result);
    }
}

size_t
moves_cache::visited_index(game_state *state, const fsm::nfa<action *> &nfa, fsm::state_id_t current_state)
{
    return (state->y() * state->width() + state->x()) * nfa.get_state_count() + current_state;
}

size_t moves_cache::new_visited(game_state *state, const fsm::nfa<action *> &nfa) {
    if(last_visited_array_index < visited.size())
    {
        visited[last_visited_array_index].front().resize(state->width() * state->height() * nfa.get_state_count());
        std::fill(visited[last_visited_array_index].front().begin(),visited[last_visited_array_index].front().end(),false);
        return last_visited_array_index++;
    }
    visited.emplace_back();
    visited.back().emplace_back(state->width() * state->height() * nfa.get_state_count(),false);
    last_visited_array_index = visited.size();
    return visited.size()-1;
}

size_t moves_cache::new_results_cache(game_state *state, const fsm::nfa<action *> &nfa) {
    if(last_results_array_index < results.size())
    {
        results[last_results_array_index].front().resize(state->width() * state->height() * nfa.get_state_count());
        std::fill(results[last_results_array_index].front().begin(),results[last_results_array_index].front().end(),false);
        return last_results_array_index++;
    }
    results.emplace_back();
    results.back().emplace_back(state->width() * state->height() * nfa.get_state_count(),false);
    last_results_array_index = results.size();
    return results.size()-1;
}

bool moves_cache::check_play(game_state *state, size_t visited_array_index, size_t results_index,
                                   const fsm::nfa<action *> &nfa, fsm::state_id_t current_state, size_t depth, bool block_started)
{
    size_t index = visited_index(state, nfa, current_state);
    if(current_state == nfa.final() || results[results_index][depth][index])
    {
        results[results_index][depth][index] = true;
        return true;
    }
    if(visited[visited_array_index][depth][index])
        return false;
    visited[visited_array_index][depth][index] = true;
    for(const auto& transition : nfa[current_state].transitions())
    {
        action_result result = transition.letter()->apply(state);
        if(result)
        {
            if(transition.letter()->is_modifier())
            {
                size_t new_depth = depth;
                size_t lazy_head = state->lazy().evaluate(state);
                if(!block_started) {
                    create_visited_layers(visited_array_index, depth + 1);
                    create_result_layers(visited_array_index, depth + 1);
                    new_depth = depth + 1;
                }
                    results[results_index][depth][index] = check_play(state, visited_array_index, results_index, nfa, transition.target(), new_depth, true);
                state->lazy().revert(state, lazy_head);
            }
            else
                results[results_index][depth][index] = check_play(state, visited_array_index, results_index, nfa, transition.target(), depth);
        }
        transition.letter()->revert(state, result);
        if(results[results_index][depth][index])
            return true;
    }
    return false;
}

std::vector<move> moves_cache::find_moves(game_state *state) {
    size_t visited_index = new_visited(state,state->get_description().get_moves_description().get_nfa());
    move empty;
    find_all_moves_rec(state, visited_index,state->get_description().get_moves_description().get_nfa(),state->get_current_state(),&empty);
    dump_visited(visited_index);
    return std::move(possible_moves);
}

bool moves_cache::check_pattern(game_state *state, const fsm::nfa<action *> &nfa, unsigned int move_pattern_index) {
    size_t visited_index = new_visited(state,state->get_description().get_moves_description().get_nfa());
    size_t results_index;
    if(move_pattern_results.find(move_pattern_index) != move_pattern_results.end())
        results_index = move_pattern_results[move_pattern_index];
    else {
        results_index = new_results_cache(state, nfa);
        move_pattern_results[move_pattern_index] = results_index;
    }
    bool result = check_play(state, visited_index, results_index, nfa, nfa.initial(), 0);
    dump_visited(visited_index);
    return result;
}

void moves_cache::create_visited_layers(size_t visited_array_index, size_t layer_depth) {
    if(visited[visited_array_index].size() > layer_depth)
    {
        visited[visited_array_index][layer_depth].resize(visited[visited_array_index].front().size());
        std::fill(visited[visited_array_index][layer_depth].begin(),visited[visited_array_index][layer_depth].end(),false);
        return;
    }
    while(visited[visited_array_index].size() <= layer_depth)
    {
        visited[visited_array_index].emplace_back(visited[visited_array_index].front().size(),false);
    }
}

void moves_cache::create_result_layers(size_t results_array_index, size_t layer_depth) {
    if(results[results_array_index].size() > layer_depth)
    {
        results[results_array_index][layer_depth].resize(visited[results_array_index].front().size());
        std::fill(results[results_array_index][layer_depth].begin(),results[results_array_index][layer_depth].end(),false);
        return;
    }
    while(results[results_array_index].size() <= layer_depth)
    {
        results[results_array_index].emplace_back(results[results_array_index].front().size(),false);
    }
}

void moves_cache::dump_visited(std::size_t visited_array_index) {
    if(visited_array_index == last_visited_array_index - 1)
    {
        last_visited_array_index--;
    }
}

void moves_cache::dump_results(std::size_t results_array_index) {
    if(results_array_index == last_results_array_index - 1)
    {
        last_results_array_index--;
    }
}

