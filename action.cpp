//
// Created by shrum on 28.12.17.
//

#include "action.h"

bool actions::shift::apply(game_state *b) {
    b->board_x += dx;
    b->board_y += dy;
    return 0 <= b->board_x && b->board_x < b->current_board.width() && 0 <= b->board_y && b->board_y < b->current_board.height();
}

void actions::shift::revert(game_state *b) {
    b->board_x -= dx;
    b->board_y -= dy;
}

bool actions::on::apply(game_state *b) {
    return pieces.find(b->current_piece()) != pieces.end();
}

void actions::on::revert(game_state *b) {}

bool actions::condition_action::apply(game_state *b) {
    /* Move patterns are not yet implemented */
    return cond->check(b);
}

void actions::condition_action::revert(game_state *b) {
}

bool actions::conditions::conjunction::check(game_state *b) {
    for(auto& cond : items)
    {
        if(!cond->check(b))
            return false;
    }
    return true;
}

bool actions::conditions::alternative::check(game_state *b) {
    for(auto& cond : items)
    {
        if(cond->check(b))
            return true;
    }
    return false;
}

bool actions::conditions::negation::check(game_state *b) {
    return !cond->check(b);
}

bool actions::conditions::less::check(game_state *b) {
    return left->value(b) < right->value(b);
}

bool actions::conditions::less_equal::check(game_state *b) {
    return left->value(b) <= right->value(b);
}

bool actions::conditions::equal::check(game_state *b) {
    return left->value(b) == right->value(b);
}

int actions::conditions::arithmetic::variable::value(game_state *b) {
    return b->sigma[variable_index];
}

int actions::conditions::arithmetic::constant::value(game_state *b) {
    return constant_value;
}


bool actions::modifier::apply(game_state *b) {
    lazy_head_before = b->get_lazy_ptr();
    b->evaluate_lazy();
    return true;
}

void actions::modifier::revert(game_state *b) {
    b->revert_lazy(lazy_head_before);
}

bool actions::modifiers::off::apply(game_state *b) {
    modifier::apply(b);
    previous_piece_id = b->current_piece();
    b->sigma[previous_piece_id]--;
    b->set_piece(piece_id);
    b->sigma[piece_id]++;
    return true;
}

void actions::modifiers::off::revert(game_state *b) {
    b->sigma[b->current_piece()]--;
    b->set_piece(previous_piece_id);
    b->sigma[b->current_piece()]++;
    modifier::revert(b);
}

bool actions::modifiers::assignment::apply(game_state *b) {
    modifier::apply(b);
    previous_value = b->sigma[variable];
    b->sigma[variable] = value;
    return true;
}

void actions::modifiers::assignment::revert(game_state *b) {
    b->sigma[variable] = previous_value;
    modifier::revert(b);
}

bool actions::modifiers::switches::player_switch::apply(game_state *b) {
    modifier::apply(b);
    previous_player_id = b->current_player;
    b->current_player = player_id;
    b->sigma[b->get_name_resolver().get_variable_id("turn")]++;
    return true;
}

void actions::modifiers::switches::player_switch::revert(game_state *b) {
    b->sigma[b->get_name_resolver().get_variable_id("turn")]--;
    b->current_player = previous_player_id;
    modifier::revert(b);
}

bool actions::modifiers::switches::semi_switch::apply(game_state *b) {
    return modifier::apply(b);
}

void actions::modifiers::switches::semi_switch::revert(game_state *b) {
    modifier::revert(b);
}

bool actions::conditions::not_equal::check(game_state *b) {
    return left->value(b) != right->value(b);
}

bool actions::conditions::move_pattern::check(game_state *b) {
    if(b->get_moves_made() != visited_moves_count || !visited)
    {
        visited = std::unique_ptr<std::unordered_map<game_state_identifier,bool,identifier_hash>>(new std::unordered_map<game_state_identifier,bool,identifier_hash>(20,identifier_hash(*b)));
    }
    game_move empty_move;
    return check_play_exists_rec(move_pattern_nfa.get_initial_id(), b, &empty_move);
}

bool actions::conditions::move_pattern::check_play_exists_rec(size_t nfa_state, game_state *state, game_move *current_move) {
    game_state_identifier id = game_state_identifier(current_move, state->board_x, state->board_y, nfa_state);
    if(visited->find(id) != visited->end())
        return (*visited)[id];

    (*visited)[id] = nfa_state == move_pattern_nfa.get_final_id();
    if(nfa_state == move_pattern_nfa.get_final_id())
    {
        return true;
    }

    for(const fsm::transition<action>& transition : move_pattern_nfa[nfa_state].get_transitions())
    {
        bool should_end = false;
        fsm::state_id_t previous_state = nfa_state;
        if(transition.is_epsilon())
        {
            nfa_state = transition.target_state_id();
            should_end = check_play_exists_rec(nfa_state, state, current_move);
            nfa_state = previous_state;
            if(should_end)
            {
                (*visited)[id] = true;
                return true;
            }
            continue;
        }
        bool success = transition.get_letter()->apply(state);
        if(success)
        {
            nfa_state = transition.target_state_id();
            if(transition.get_letter()->is_modifier())
            {
                game_move new_move(*current_move);
                should_end = check_play_exists_rec(nfa_state,state, &new_move);
            }
            else
            {
                should_end = check_play_exists_rec(nfa_state,state, current_move);
            }
            nfa_state = previous_state;
        }
        transition.get_letter()->revert(state);
        if(should_end)
        {
            (*visited)[id] = true;
            return true;
        }
    }
    (*visited)[id] = false;
    return false;
}

