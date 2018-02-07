//
// Created by shrum on 14.01.18.
//

#include "search_context.h"
#include "game_state.h"
#include <algorithm>



void search_context::find_all_moves_rec(size_t visited_array_index, const fsm::nfa<const action*> &nfa,
                                           fsm::state_id_t current_state, move *move, bool block_started)
{
    size_t index = visited_index(nfa, current_state);
    size_t depth = move->get_blocks().size();
    if(visited[visited_array_index][depth][index])
        return;
    visited[visited_array_index][depth].set(index);
    if((ssize_t)depth == max_depth)
        return;
    for(const auto& transition : nfa[current_state].transitions())
    {
        action_result result = transition.letter()->apply(calculation_state);
        if(result)
        {
            if(transition.letter()->is_modifier())
            {
                if(!block_started) {
                    move->add_block(calculation_state->x(), calculation_state->y(), transition.letter()->get_index());
                    create_visited_layers(visited_array_index, depth+1);
                    invalidate_results();
                }
                if(transition.letter()->is_switch())
                    possible_moves.push_back(*move);
                else
                {
                    find_all_moves_rec(visited_array_index, nfa, transition.target(), move, true);
                }
                if(!block_started) {
                    move->pop_block();
                }
            }
            else
                find_all_moves_rec(visited_array_index, nfa, transition.target(), move);
        }
        transition.letter()->revert(calculation_state, result);
    }
}

size_t
search_context::visited_index(const fsm::nfa<const action*> &nfa, fsm::state_id_t current_state)
{
    return (calculation_state->y() * calculation_state->width() + calculation_state->x()) * nfa.get_state_count() + current_state;
}

size_t search_context::new_visited(const fsm::nfa<const action*> &nfa) {
    if(last_visited_array_index < visited.size())
    {
        visited[last_visited_array_index].front().resize(std::max(visited[last_visited_array_index].front().size(),
                                                                  calculation_state->width() * calculation_state->height() * nfa.get_state_count()));
        visited[last_visited_array_index].front().reset();
        return last_visited_array_index++;
    }
    visited.emplace_back();
    visited.back().emplace_back(calculation_state->width() * calculation_state->height() * nfa.get_state_count(),false);
    last_visited_array_index = visited.size();
    return visited.size()-1;
}

size_t search_context::new_results_cache(const fsm::nfa<const action*> &nfa) {
    if(last_results_array_index < results.size())
    {
        results[last_results_array_index].front().resize(std::max(results[last_results_array_index].front().size(),
                                                                  calculation_state->width() * calculation_state->height() * nfa.get_state_count()));
        results[last_results_array_index].front().reset();
        return last_results_array_index++;
    }
    results.emplace_back();
    results.back().emplace_back(calculation_state->width() * calculation_state->height() * nfa.get_state_count(),false);
    last_results_array_index = results.size();
    return results.size()-1;
}

bool search_context::check_play(size_t visited_array_index, size_t results_index,
                                   const fsm::nfa<const action*> &nfa, fsm::state_id_t current_state, size_t depth, bool block_started)
{
    size_t index = visited_index(nfa, current_state);
    if(current_state == nfa.final() || results[results_index][depth][index])
    {
        results[results_index][depth].set(index);
        return true;
    }
    if(visited[visited_array_index][depth][index])
        return false;
    visited[visited_array_index][depth].set(index);
    if((ssize_t)depth == max_depth)
        return false;
    for(const auto& transition : nfa[current_state].transitions())
    {
        action_result result = transition.letter()->apply(calculation_state);
        if(result)
        {
            if(transition.letter()->is_modifier())
            {
                size_t new_depth = depth;
                if(!block_started) {
                    create_visited_layers(visited_array_index, depth + 1);
                    create_result_layers(visited_array_index, depth + 1);
                    invalidate_results(visited_array_index);
                    new_depth = depth + 1;
                }
                if(check_play(visited_array_index, results_index, nfa, transition.target(), new_depth, true))
                    results[results_index][depth].set(index);
            }
            else
                if(check_play(visited_array_index, results_index, nfa, transition.target(), depth))
                    results[results_index][depth].set(index);
        }
        transition.letter()->revert(calculation_state, result);
        if(results[results_index][depth][index])
            return true;
    }
    return false;
}

std::vector<move> search_context::find_moves(game_state *state, ssize_t maximal_depth) {
    calculation_state = state;
    if(maximal_depth >= 0)
        max_depth = (size_t) maximal_depth;
    size_t visited_index = new_visited(state->get_description().get_moves_description().get_nfa());
    move empty;
    find_all_moves_rec(visited_index,state->get_description().get_moves_description().get_nfa(),state->get_current_state(),&empty);
    dump_visited(visited_index);
    calculation_state = nullptr;
    return std::move(possible_moves);
}

bool search_context::check_pattern(game_state *state, const fsm::nfa<const action*> &nfa, unsigned int move_pattern_index, ssize_t maximal_depth) {
    calculation_state = state;
    bool result = check_pattern(nfa, move_pattern_index, maximal_depth);
    calculation_state = nullptr;
    return result;
}

void search_context::create_visited_layers(size_t visited_array_index, size_t layer_depth) {
    if(visited[visited_array_index].size() > layer_depth)
    {
        visited[visited_array_index][layer_depth].resize(visited[visited_array_index].front().size());
        visited[visited_array_index][layer_depth].reset();
        return;
    }
    while(visited[visited_array_index].size() <= layer_depth)
    {
        visited[visited_array_index].emplace_back(visited[visited_array_index].front().size(),false);
    }
}

void search_context::create_result_layers(size_t results_array_index, size_t layer_depth) {
    if(results[results_array_index].size() > layer_depth)
    {
        results[results_array_index][layer_depth].resize(results[results_array_index].front().size());
        results[results_array_index][layer_depth].reset();
        return;
    }
    while(results[results_array_index].size() <= layer_depth)
    {
        results[results_array_index].emplace_back(results[results_array_index].front().size(),false);
    }
}

void search_context::dump_visited(std::size_t visited_array_index) {
    if(visited_array_index == last_visited_array_index - 1)
    {
        last_visited_array_index--;
    }
}

std::vector<move> search_context::find_first_move(game_state *state, ssize_t maximal_depth) {
    calculation_state = state;
    if(maximal_depth >= 0)
        max_depth = (size_t) maximal_depth;
    size_t visited_index = new_visited(state->get_description().get_moves_description().get_nfa());
    move empty;
    find_first_move_rec(visited_index,state->get_description().get_moves_description().get_nfa(),state->get_current_state(),&empty);
    dump_visited(visited_index);
    calculation_state = nullptr;
    return std::move(possible_moves);
}

bool search_context::find_first_move_rec(std::size_t visited_array_index, const fsm::nfa<const action*> &nfa,
                                      fsm::state_id_t current_state, move *move, bool block_started) {
    size_t index = visited_index(nfa, current_state);
    size_t depth = move->get_blocks().size();
    if(visited[visited_array_index][depth][index])
        return false;
    visited[visited_array_index][depth].set(index);
    if((ssize_t)depth == max_depth)
        return false;
    bool rec_result = false;
    for(const auto& transition : nfa[current_state].transitions())
    {
        action_result result = transition.letter()->apply(calculation_state);
        if(result)
        {
            if(transition.letter()->is_modifier())
            {
                if(!block_started) {
                    move->add_block(calculation_state->x(), calculation_state->y(), transition.letter()->get_index());
                    create_visited_layers(visited_array_index, depth+1);
                    invalidate_results();
                }
                if(transition.letter()->is_switch()) {
                    possible_moves.push_back(*move);
                    rec_result = true;
                }
                else
                {
                    rec_result = find_first_move_rec(visited_array_index, nfa, transition.target(), move, true);
                }
                if(!block_started) {
                    move->pop_block();
                }
            }
            else
                rec_result = find_first_move_rec(visited_array_index, nfa, transition.target(), move);
        }
        transition.letter()->revert(calculation_state, result);
        if(rec_result)
            return true;
    }
    return false;
}

bool search_context::check_pattern(const fsm::nfa<const action*> &nfa, unsigned int move_pattern_index, ssize_t maximal_depth) {
    if(maximal_depth >= 0)
        max_depth = (size_t) maximal_depth;
    size_t visited_index = new_visited(nfa);
    size_t results_index;
    if(move_pattern_results.find(move_pattern_index) != move_pattern_results.end())
        results_index = move_pattern_results[move_pattern_index];
    else {
        results_index = new_results_cache(nfa);
        move_pattern_results[move_pattern_index] = results_index;
    }
    bool result = check_play(visited_index, results_index, nfa, nfa.initial(), 0);
    dump_visited(visited_index);
    return result;
}


