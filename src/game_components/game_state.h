//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_GAME_STATE_H
#define RBGGAMEMANAGER_GAME_STATE_H


#include <utility>

#include "game_description.h"
#include "lazy_evaluator.h"
#include "search_context.h"

class move_revert_information
{
    std::vector<action_result> actions_results;
    size_t previous_x, previous_y;
    fsm::state_id_t previous_state;
    std::vector<action_application> applied_modifiers;
public:
    move_revert_information(std::vector<action_result> actions_results, size_t previous_x, size_t previous_y,
                                fsm::state_id_t previous_state, std::vector<action_application> applied_modifiers)
            : actions_results(std::move(actions_results)), previous_x(previous_x),
              previous_y(previous_y), previous_state(previous_state), applied_modifiers(std::move(applied_modifiers)) {}

    const std::vector<action_result> &get_actions_results() const {
        return actions_results;
    }

    const std::vector<action_application> &get_applied_modifiers() const {
        return applied_modifiers;
    }

    size_t get_previous_x() const {
        return previous_x;
    }

    size_t get_previous_y() const {
        return previous_y;
    }

    fsm::state_id_t get_previous_state() const {
        return previous_state;
    }
};

class game_state {
    const game_description& parent;

    lazy_evaluator lazy_controller;

    board current_board;
    size_t current_x,current_y;
    fsm::state_id_t current_state;

    std::vector<int> sigma;
    token_id_t current_player;

    search_context* current_search;

    void inc_turn() { sigma[parent.get_turn_id()]++; }
    void dec_turn() { sigma[parent.get_turn_id()]--; }

    void set_piece(token_id_t piece)
    {
        sigma[current_piece()]--;
        current_board(current_x,current_y) = piece;
        sigma[piece]++;
    }

    lazy_evaluator& lazy()
    {
        return lazy_controller;
    }

    bool is_used_in_search()
    {
        return current_search != nullptr;
    }

    search_context& get_search_context()
    {
        return *current_search;
    }

    action_result apply_action_application(const action_application& application);
    void revert_action_application(const action_application& application, const action_result& application_result);
public:
    explicit game_state(const game_description& description)
        : parent(description),
          current_board(description.get_initial_board()),
          current_x(0), current_y(0),
          current_state(description.get_moves_description().get_nfa().initial()),
          sigma(description.get_variables_count()),
          current_player(description.get_deterministic_keeper_player_id())
    {
        for(size_t y = 0; y < current_board.height(); y++)
        {
            for(size_t x = 0; x < current_board.width(); x++)
            {
                sigma[current_board(x,y)]++;
            }
        }
        sigma[get_description().get_turn_id()] = 0;
    }

    void reset()
    {
        current_board = parent.get_initial_board();
        current_x = 0;
        current_y = 0;
        current_state = parent.get_moves_description().get_nfa().initial();
        std::fill(sigma.begin(),sigma.end(),0);
        current_player = parent.get_deterministic_keeper_player_id();
    }

    fsm::state_id_t get_current_state() const
    {
        return current_state;
    }

    token_id_t current_piece() const
    {
        return current_board(current_x,current_y);
    }

    const game_description& get_description() const
    {
        return parent;
    }

    int value(token_id_t variable) const
    {
        return sigma[variable];
    }

    const board& get_board() const
    {
        return current_board;
    }

    size_t x() const
    {
        return current_x;
    }

    size_t y() const
    {
        return current_y;
    }

    size_t width() const
    {
        return current_board.width();
    }

    size_t height() const
    {
        return current_board.height();
    }

    int turn() const
    {
        return sigma[parent.get_turn_id()];
    }

    token_id_t player() const
    {
        return current_player;
    }

    void make_move(const move& move)
    {
        for(const auto& block : move.get_blocks())
        {
            current_state = parent.get_moves_description().get_corresponding_state(block.get_block_id());
            current_x = block.x();
            current_y = block.y();
            while(parent.get_moves_description().get_nfa()[current_state].transitions().size() == 1 &&
                    parent.get_moves_description().get_nfa()[current_state].transitions().front().letter()->get_index() == block.get_block_id())
            {
                parent.get_moves_description().get_nfa()[current_state].transitions().front().letter()->apply(this);
                current_state = parent.get_moves_description().get_nfa()[current_state].transitions().front().target();
            }
        }
        lazy_controller.clear();
    }

    move_revert_information make_revertible_move(const move& move)
    {
        std::vector<action_result> results;
        std::vector<action_application> applied_modifiers;
        size_t previous_x = current_x;
        size_t previous_y = current_y;
        fsm::state_id_t previous_state = current_state;
        for(const auto& block : move.get_blocks())
        {
            current_state = parent.get_moves_description().get_corresponding_state(block.get_block_id());
            current_x = block.x();
            current_y = block.y();
            while(parent.get_moves_description().get_nfa()[current_state].transitions().size() == 1 &&
                  parent.get_moves_description().get_nfa()[current_state].transitions().front().letter()->get_index() == block.get_block_id())
            {
                results.push_back(parent.get_moves_description().get_nfa()[current_state].transitions().front().letter()->apply(this));
                applied_modifiers.emplace_back(current_x, current_y, parent.get_moves_description().get_nfa()[current_state].transitions().front().letter());
                current_state = parent.get_moves_description().get_nfa()[current_state].transitions().front().target();
            }
        }
        return {std::move(results), previous_x, previous_y, previous_state, std::move(applied_modifiers)};
    }

    void revert_move(const move_revert_information& information)
    {
        for(ssize_t i = information.get_applied_modifiers().size()-1; i >= 0; i--)
        {
            const auto& action = information.get_applied_modifiers()[i];
            revert_action_application(action, information.get_actions_results()[i]);
        }
        current_x = information.get_previous_x();
        current_y = information.get_previous_y();
        current_state = information.get_previous_state();
    }

    std::vector<move> find_moves(search_context* context, ssize_t max_depth=-1)
    {
        current_search = context;
        auto result = current_search->find_moves(this, max_depth);
        current_search = nullptr;
        return std::move(result);
    }

    std::vector<move> find_first_move(search_context* context, ssize_t max_depth=-1)
    {
        current_search = context;
        auto result = current_search->find_first_move(this, max_depth);
        current_search = nullptr;
        return std::move(result);
    }

    // Lazy controller is separated from main class
    friend class lazy_evaluator;

    // Modifying actions
    friend class actions::shift;
    friend class actions::off;
    friend class actions::player_switch;
    friend class actions::semi_switch;
    friend class actions::assignment;
    friend class actions::lazy;

    // Move pattern will be able to use the state to check the condition
    // This will speed up things. (We do not have to make a copy.)
    friend class conditions::move_pattern;
};

std::ostream& operator<<(std::ostream& s,const game_state& state);

#endif //RBGGAMEMANAGER_GAME_STATE_H
