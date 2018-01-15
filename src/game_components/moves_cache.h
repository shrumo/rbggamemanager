//
// Created by shrum on 14.01.18.
//

#ifndef RBGGAMEMANAGER_MOVES_INFORMATION_H
#define RBGGAMEMANAGER_MOVES_INFORMATION_H

#include <vector>
#include "../game_nfa/automaton.h"
#include "../utils/fast_bit_vector.h"
#include <unordered_map>

class game_state;
class action;

class block_application
{
    std::size_t applied_x,applied_y;
    std::size_t block_id;
public:
    block_application(std::size_t applied_x, std::size_t applied_y, std::size_t block_id)
            : applied_x(applied_x), applied_y(applied_y),
              block_id(block_id) {}

    std::size_t x() const
    {
        return applied_x;
    }

    std::size_t y() const
    {
        return applied_y;
    }

    std::size_t get_block_id() const
    {
        return block_id;
    }
};

class move
{
    std::vector<block_application> blocks;
public:
    move() = default;

    template<typename ...Args>
    void add_block(Args&&... args)
    {
        blocks.emplace_back(std::forward<Args>(args)...);
    }

    void pop_block()
    {
        blocks.pop_back();
    }

    const std::vector<block_application>& get_blocks() const
    {
        return blocks;
    }
};

class moves_cache {
    std::vector<move> possible_moves;

    std::vector< std::vector< fast_bit_vector > > visited;
    std::vector< std::vector< fast_bit_vector > > results;

    std::size_t last_visited_array_index;
    std::size_t last_results_array_index;

    std::size_t new_visited(game_state* state, const fsm::nfa<action *> &nfa);
    std::size_t new_results_cache(game_state* state, const fsm::nfa<action *> &nfa);

    void dump_visited(std::size_t visited_array_index);
    void dump_results(std::size_t results_array_index);

    void create_visited_layers(std::size_t visited_array_index, std::size_t layer_depth);
    void create_result_layers(std::size_t results_array_index, std::size_t layer_depth);

    std::unordered_map<unsigned int, std::size_t> move_pattern_results;

    void find_all_moves_rec(game_state* state, std::size_t visited_array_index, const fsm::nfa<action*>& nfa, fsm::state_id_t current_state, move* move, bool block_started=false);
    bool check_play(game_state* state, std::size_t visited_array_index, std::size_t results_index, const fsm::nfa<action*>& nfa, fsm::state_id_t current_state, std::size_t depth, bool block_started=false);

    std::size_t visited_index(game_state* state, const fsm::nfa<action *> &nfa, fsm::state_id_t current_state);
public:
    moves_cache();

    void clear()
    {
        last_visited_array_index=0;
        last_results_array_index=0;
        move_pattern_results.clear();
    }

    std::vector<move> find_moves(game_state* state);
    bool check_pattern(game_state* state, const fsm::nfa<action*>& nfa, unsigned int move_pattern_index);
};


#endif //RBGGAMEMANAGER_MOVES_INFORMATION_H
