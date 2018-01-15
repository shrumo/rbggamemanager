//
// Created by shrum on 12.01.18.
//

#include "game_state.h"

action_result game_state::apply_action_application(const action_application &application) {
    std::size_t old_x = current_x;
    std::size_t old_y = current_y;
    set_pos(application.x(), application.y());
    auto res = application.get_action()->apply(this);
    set_pos(old_x,old_y);
    return res;
}

void
game_state::revert_action_application(const action_application &application, const action_result& application_result) {
    std::size_t old_x = current_x;
    std::size_t old_y = current_y;
    set_pos(application.x(), application.y());
    application.get_action()->revert(this,application_result);
    set_pos(old_x,old_y);
}
