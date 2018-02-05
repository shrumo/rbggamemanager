//
// Created by shrum on 12.01.18.
//

#include "action.h"
#include "../game_components/game_state.h"

action_result actions::shift::apply(game_state *b) const {
    b->current_x += dx;
    b->current_y += dy;
    return b->x()< b->width() && b->y() < b->height();
}

void actions::shift::revert(game_state *b, const action_result&) const {
    b->current_x-=dx;
    b->current_y-=dy;
}

action_result actions::on::apply(game_state *b) const {
    return {pieces[b->current_piece()]};
}

action_result actions::off::apply(game_state *b) const {
    b->lazy().evaluate_lazy_reversible(b);
    size_t previous_piece = b->current_piece();
    b->set_piece(piece);
    return {true, previous_piece};
}

void actions::off::revert(game_state *b, const action_result &apply_result) const {
    b->set_piece(apply_result.get_revert_piece());
    b->lazy().revert_last_lazy_evaluation(b);
}

action_result actions::player_switch::apply(game_state *b) const {
    b->lazy().evaluate_lazy_reversible(b);
    size_t previous_player = b->player();
    b->current_player = player;
    return {true, previous_player};
}

void actions::player_switch::revert(game_state *b, const action_result &apply_result) const {
    b->current_player = apply_result.get_revert_player();
    b->lazy().revert_last_lazy_evaluation(b);
}

action_result actions::assignment::apply(game_state *b) const {
    b->lazy().evaluate_lazy_reversible(b);
    int previous_value = b->value(variable);
    b->sigma[variable] = value->value(b);
    return {true, previous_value};
}

void actions::assignment::revert(game_state *b, const action_result &apply_result) const {
    b->sigma[variable] = apply_result.get_revert_value();
    b->lazy().revert_last_lazy_evaluation(b);
}

action_result actions::lazy::apply(game_state *b) const {
    b->lazy().add_lazy_action(b->x(),b->y(),lazy_action);
    return true;
}

void actions::lazy::revert(game_state *b, const action_result &) const {
    b->lazy().pop_lazy_action();
}

action_result actions::incrementation::apply(game_state *b) const {
    b->sigma[variable]++;
    b->sigma[variable] %= b->get_description().get_bound();
    return true;
}

void actions::incrementation::revert(game_state *b, const action_result &) const {
    b->sigma[variable]--;
    b->sigma[variable] += b->get_description().get_bound();
    b->sigma[variable] %= b->get_description().get_bound();
}

action_result actions::decrementation::apply(game_state *b) const {
    b->sigma[variable]--;
    b->sigma[variable] += b->get_description().get_bound();
    b->sigma[variable] %= b->get_description().get_bound();
    return true;
}

void actions::decrementation::revert(game_state *b, const action_result &) const {
    b->sigma[variable]++;
    b->sigma[variable] %= b->get_description().get_bound();
}
