//
// Created by shrum on 12.01.18.
//

#include "action.h"
#include "../game_components/game_state.h"

action_result actions::shift::apply(game_state *b) const {
    b->change_pos(dx,dy);
    return b->x()< b->width() && b->y() < b->height();
}

void actions::shift::revert(game_state *b, const action_result&) const {
    b->change_pos(-dx,-dy);
}

action_result actions::on::apply(game_state *b) const {
    return action_result(pieces[b->current_piece()]);
}

action_result actions::off::apply(game_state *b) const {
    size_t previous_piece = b->current_piece();
    b->set_piece(piece);
    return {true, previous_piece};
}

void actions::off::revert(game_state *b, const action_result &apply_result) const {
    b->set_piece(apply_result.get_revert_piece());
}

action_result actions::player_switch::apply(game_state *b) const {
    size_t previous_player = b->player();
    b->set_player(player);
    b->inc_turn();
    return {true, previous_player};
}

void actions::player_switch::revert(game_state *b, const action_result &apply_result) const {
    b->dec_turn();
    b->set_player(apply_result.get_revert_player());
}

action_result actions::assignment::apply(game_state *b) const {
    int previous_value = b->value(variable);
    b->set_value(variable, value);
    return {true, previous_value};
}

void actions::assignment::revert(game_state *b, const action_result &apply_result) const {
    b->set_value(variable, apply_result.get_revert_value());
}

action_result actions::lazy::apply(game_state *b) const {
    b->lazy().add_lazy_action(b->x(),b->y(),lazy_action);
    return true;
}

void actions::lazy::revert(game_state *b, const action_result &) const {
    b->lazy().pop_lazy_action();
}
