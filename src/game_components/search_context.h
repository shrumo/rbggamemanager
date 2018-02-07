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

    bool operator!=(const block_application& b) const /* TODO(shrum): Make the operators right. */
    {
        return x() != b.x() || y() != b.y() || block_id != b.block_id;
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

    bool operator==(const move& b) const
    {
        const auto& a_blocks = get_blocks();
        const auto& b_blocks = b.get_blocks();
        if(a_blocks.size() != b_blocks.size())
            return false;
        for(size_t i = 0; i < a_blocks.size(); i++)
        {
            if(a_blocks[i] != b_blocks[i])
            {
                return false;
            }
        }
        return true;
    }
};

using move_type = move;

namespace std
{
    template <>
    struct hash<move_type>
    {
        std::size_t operator()(const move_type& m) const
        {
            std::size_t result = 0;
            for(const auto& block : m.get_blocks())
            {
                result *= 2579;
                result += (block.y() * 31 + block.x()) * 83 + block.get_block_id();
            }
            return result;
        }
    };
}

class search_context {
    std::vector<move> possible_moves;
    ssize_t max_depth;

    std::vector< std::vector< fast_bit_vector > > visited;
    std::vector< std::vector< fast_bit_vector > > results;

    std::size_t last_visited_array_index;
    std::size_t last_results_array_index;

    std::size_t new_visited(const fsm::nfa<const action*> &nfa);
    std::size_t new_results_cache(const fsm::nfa<const action*> &nfa);

    void dump_visited(std::size_t visited_array_index);

    void create_visited_layers(std::size_t visited_array_index, std::size_t layer_depth);
    void create_result_layers(std::size_t results_array_index, std::size_t layer_depth);

    std::unordered_map<unsigned int, std::size_t> move_pattern_results;

    void find_all_moves_rec(std::size_t visited_array_index, const fsm::nfa<const action*>& nfa, fsm::state_id_t current_state, move* move, bool block_started=false);
    bool find_first_move_rec(std::size_t visited_array_index, const fsm::nfa<const action*>& nfa, fsm::state_id_t current_state, move* move, bool block_started=false);
    bool check_play(std::size_t visited_array_index, std::size_t results_index, const fsm::nfa<const action*>& nfa, fsm::state_id_t current_state, std::size_t depth, bool block_started=false);

    std::size_t visited_index(const fsm::nfa<const action*> &nfa, fsm::state_id_t current_state);

    game_state* calculation_state;
public:
    search_context()
            :
            max_depth(-1),
            last_visited_array_index(0),
            last_results_array_index(0)
    {
    }

    bool is_used()
    {
        return calculation_state != nullptr;
    }

    void clear()
    {
        last_visited_array_index=0;
        last_results_array_index=0;
        move_pattern_results.clear();
    }

    void invalidate_results(ssize_t omit=-1)
    {
        for(size_t i = 0; i < results.size(); i++)
        {
            if((ssize_t )i != omit)
            {
                for(auto& depth_results : results[i])
                {
                    depth_results.reset();
                }
            }
        }
    }

    std::vector<move> find_moves(game_state* state, ssize_t max_depth=-1);
    std::vector<move> find_first_move(game_state* state, ssize_t max_depth=-1);
    bool check_pattern(game_state* state, const fsm::nfa<const action*>& nfa, unsigned int move_pattern_index, ssize_t max_depth=-1);
    bool check_pattern(const fsm::nfa<const action*>& nfa, unsigned int move_pattern_index, ssize_t max_depth=-1);

};


#endif //RBGGAMEMANAGER_MOVES_INFORMATION_H
