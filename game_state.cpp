//
// Created by shrum on 02.01.18.
//

#include "action.h"
#include "dispatchers.h"

void game_state::revert_lazy(size_t ptr) {
    if(evaluating_lazy)
        return;
    evaluating_lazy = true;
    while(lazy_head > ptr)
    {
        lazy_head--;
        lazy_actions[lazy_head]->revert(this);
    }
    evaluating_lazy = false;
}

void game_state::evaluate_lazy() {
    if(evaluating_lazy)
        return;
    evaluating_lazy = true;
    while(lazy_head < lazy_actions.size())
    {
        lazy_actions[lazy_head]->apply(this);
        lazy_head++;
    }
    evaluating_lazy = false;
}

size_t game_state::get_lazy_ptr() const {
    return lazy_head;
}

void game_state::set_piece(token_id_t piece_id) {
    current_board(board_x,board_y) = piece_id;
}

token_id_t game_state::current_piece() const {
    return current_board(board_x,board_y);
}

const name_resolver& game_state::get_name_resolver() const {
    return resolver;
}

void game_state::add_lazy_action(action *action) {
    lazy_actions.emplace_back(action);
}

void game_state::pop_lazy_action() {
    lazy_actions.pop_back();
}

game_state::game_state(const rbg_parser::parsed_game &parsed_game)
    : resolver(parsed_game), reg(), nfa(&reg), modifier_states(), lazy_actions(),
      lazy_head(0),
      current_nfa_state(0),
      current_board(parsed_game.get_board(), resolver),
      board_x(0),
      board_y(0),
      sigma(resolver.variables_count(),0),
      current_player(resolver.get_player_id(parsed_game.get_declarations().get_first_player().to_string())),
      evaluating_lazy(false)
{
    game_nfa_dispatcher dispatcher(&reg, resolver);
    parsed_game.get_moves()->accept(dispatcher);
    nfa = dispatcher.get_result();
    modifier_states = dispatcher.get_modifier_states();
}


void find_all_moves_rec(game_state* state, game_move* current_move,
                        std::vector<game_move>* result, std::unordered_set<game_state_identifier,identifier_hash >* visited)
{
    game_state_identifier id = create_identifier(*state, current_move);
    if(visited->find(id) != visited->end())
        return;
    visited->insert(id);
    for(const fsm::transition<action>& transition : state->nfa[state->current_nfa_state].get_transitions())
    {
        fsm::state_id_t previous_state = state->current_nfa_state;
        if(transition.is_epsilon())
        {
            state->current_nfa_state = transition.target_state_id();
            find_all_moves_rec(state, current_move, result, visited);
            state->current_nfa_state = previous_state;
            continue;
        }
        bool success = transition.get_letter()->apply(state);
        if(success)
        {
            state->current_nfa_state = transition.target_state_id();
            if(transition.get_letter()->is_modifier())
            {
                game_move new_move(*current_move);
                size_t modifier_index = dynamic_cast<actions::modifier*>(transition.get_letter())->get_index();
                new_move.segments.emplace_back(state->board_x, state->board_y, modifier_index);
                if(transition.get_letter()->is_switch())
                {
                    result->emplace_back(new_move);
                }
                else
                {
                    find_all_moves_rec(state, &new_move, result, visited);
                }
            }
            else
            {
                find_all_moves_rec(state, current_move, result, visited);
            }
            state->current_nfa_state = previous_state;
        }
        transition.get_letter()->revert(state);
    }
}

std::vector<game_move> game_state::find_all_moves(){
    std::vector<game_move> result;
    game_move empty_move;
    std::unordered_set<game_state_identifier,identifier_hash > visited(current_board.width() * current_board.height() * 4,identifier_hash(*this));
    find_all_moves_rec(this, &empty_move, &result, &visited);
    return result;
}

const fsm::state_register<action> &game_state::get_state_register() const {
    return reg;
}

fsm::state_id_t game_state::get_current_nfa_state() const {
    return current_nfa_state;
}

void game_state::make_move(const game_move &move) {
    for(const auto& segment : move.segments)
    {
        board_x = segment.board_x;
        board_y = segment.board_y;
        current_nfa_state = modifier_states[segment.modifier_index];
        const game_transition& transition = nfa[current_nfa_state].get_transitions()[0];
        transition.get_letter()->apply(this);
        current_nfa_state = transition.target_state_id();
    }
    evaluate_lazy();
    lazy_actions.clear();
    lazy_head = 0;
    moves_made++;
}

int game_state::turn() {
    return sigma[resolver.get_variable_id("turn")];
}

size_t game_state::get_moves_made() {
    return moves_made;
}

bool operator==(const game_state_identifier &a, const game_state_identifier &b) {
    return a.applied_move == b.applied_move && a.board_x == b.board_x && a.board_y == b.board_y && a.current_nfa_state == b.current_nfa_state;
}


game_state_identifier create_identifier(const game_state &state, game_move *move) {
    return {move, state.board_x, state.board_y, state.get_current_nfa_state()};
}
