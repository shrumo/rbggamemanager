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
    return left->check(b) && right->check(b);
}

bool actions::conditions::alternative::check(game_state *b) {
    return left->check(b) || right->check(b);
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
    b->set_piece(piece_id);
    return true;
}

void actions::modifiers::off::revert(game_state *b) {
    b->set_piece(previous_piece_id);
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
