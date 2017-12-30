//
// Created by shrum on 28.12.17.
//

#include "action.h"

bool actions::shift::apply(game_state *b) {
    return false;
}

bool actions::on::apply(game_state *b) {
    return false;
}

bool actions::condition_action::apply(game_state *b) {
    return false;
}

bool actions::conditions::conjunction::check(game_state *b) {
    return false;
}

bool actions::conditions::alternative::check(game_state *b) {
    return false;
}

bool actions::conditions::negation::check(game_state *b) {
    return false;
}

bool actions::conditions::less::check(game_state *b) {
    return false;
}

bool actions::conditions::less_equal::check(game_state *b) {
    return false;
}

bool actions::conditions::equal::check(game_state *b) {
    return false;
}

int actions::conditions::arithmetic::variable::value(game_state *b) {
    return 0;
}

int actions::conditions::arithmetic::constant::value(game_state *b) {
    return 0;
}

bool actions::off::apply(game_state *b) {
    return false;
}

bool actions::lazy_off::apply(game_state *b) {
    return false;
}

bool actions::player_switch::apply(game_state *b) {
    return false;
}

bool actions::semi_switch::apply(game_state *b) {
    return false;
}

bool actions::assignment::apply(game_state *b) {
    return false;
}

bool actions::lazy_assignment::apply(game_state *b) {
    return false;
}
