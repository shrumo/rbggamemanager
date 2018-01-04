//
// Created by shrum on 02.01.18.
//

#ifndef RBGGAMEMANAGER_GAME_STATE_H
#define RBGGAMEMANAGER_GAME_STATE_H

#include <vector>
#include <memory>
#include "board.h"
#include "automaton.h"

class action;
class game_description;

using game_nfa = fsm::nfa<action>;
using game_transition = fsm::transition<action>;
class game_move;
class game_state_identifier;
class identifier_hash;

class game_state
{
    name_resolver resolver;
    fsm::state_register<action> reg;

    game_nfa nfa;
    std::vector<fsm::state_id_t> modifier_states;

    std::vector<action*> lazy_actions;
    bool evaluating_lazy;
    size_t lazy_head;

    fsm::state_id_t current_nfa_state;

    size_t moves_made;
public:
    board current_board;
    size_t board_x;
    size_t board_y;
    std::vector<int> sigma;
    token_id_t current_player;

    int turn();
    size_t get_moves_made();

    game_state(const rbg_parser::parsed_game& parsed_game);

    game_state(const game_state& g) = delete;

    token_id_t current_piece() const;
    void set_piece(token_id_t piece_id);

    void add_lazy_action(action* action);
    void pop_lazy_action();

    size_t get_lazy_ptr() const;
    void evaluate_lazy();
    void revert_lazy(size_t ptr);

    const name_resolver& get_name_resolver() const;

    fsm::state_id_t get_current_nfa_state() const;

    const fsm::state_register<action>& get_state_register() const;

    std::vector<game_move> find_all_moves();
    friend void find_all_moves_rec(game_state* state, game_move* current_move,
                                   std::vector<game_move>* result, std::unordered_set<game_state_identifier,identifier_hash >* visited);
    void make_move(const game_move& move);
};

std::ostream& operator<<(std::ostream& s,const game_state& state);

class move_segment
{
public:

    size_t board_x;
    size_t board_y;
    size_t modifier_index;
    move_segment(size_t board_x, size_t board_y, size_t modifier_index)
            : board_x(board_x), board_y(board_y), modifier_index(modifier_index)
    {}
};

class game_move
{
public:

    std::vector<move_segment> segments;
    game_move() = default;

    friend class game_state;
    friend class identifier_hash;
    friend void find_all_moves_rec(game_state* state, game_move* current_move,
                                         std::vector<game_move>* result, std::unordered_set<game_state_identifier,identifier_hash >* visited);
};

class game_state_identifier
{
    game_move* applied_move;
    size_t board_x;
    size_t board_y;
    fsm::state_id_t current_nfa_state;
public:
    game_state_identifier(game_move* applied_move, size_t board_x, size_t board_y,
                          fsm::state_id_t current_nfa_state) : applied_move(applied_move), board_x(board_x),
                                                               board_y(board_y), current_nfa_state(current_nfa_state)
    {}
    friend class identifier_hash;

    friend bool operator==(const game_state_identifier& a, const game_state_identifier& b);
};

bool operator==(const game_state_identifier& a, const game_state_identifier& b);

game_state_identifier create_identifier(const game_state& state, game_move* move);

class identifier_hash
{
    const game_state& parent_state;
public:
    identifier_hash(const game_state& parent_state)
            : parent_state(parent_state)
    {}

    size_t operator()(const game_state_identifier& identifier) const
    {
        size_t result = identifier.board_y * parent_state.current_board.width() + identifier.board_x;
        result = result * parent_state.get_state_register().size() + identifier.current_nfa_state;
        uintptr_t ad = (uintptr_t)identifier.applied_move;
        result *= 1 << 5;
        result += (size_t)(ad ^ (ad >> 16));
        return result;
    }
};



#endif //RBGGAMEMANAGER_GAME_STATE_H
