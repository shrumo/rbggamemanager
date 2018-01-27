//
// Created by shrum on 12.01.18.
//

#include <iomanip>
#include "game_state.h"

action_result game_state::apply_action_application(const action_application &application) {
    std::size_t old_x = current_x;
    std::size_t old_y = current_y;
    current_x = application.x();
    current_y = application.y();
    auto res = application.get_action()->apply(this);
    current_x = old_x;
    current_y = old_y;
    return res;
}

void
game_state::revert_action_application(const action_application &application, const action_result& application_result) {
    std::size_t old_x = current_x;
    std::size_t old_y = current_y;
    current_x = application.x();
    current_y = application.y();
    application.get_action()->revert(this,application_result);
    current_x = old_x;
    current_y = old_y;
}

std::ostream &operator<<(std::ostream &s, const game_state &state) {
    s << "Player: " << state.get_description().get_resolver().name(state.player()) << "\n";
    s << "Variables: ";
    for(token_id_t i = 0; i < state.get_description().get_variables_count() ; i++)
    {
        s << state.get_description().get_resolver().name(i) << ":" << state.value(i);
        if(i != state.get_description().get_variables_count()-1)
            s << ", ";
    }
    s << "\n";
    s << "Position: " << state.x() << ", " << state.y() << "[" << state.get_current_state() << "]\n";
    for(size_t y = 0; y < state.height(); y++)
    {
        for(size_t x = 0; x < state.width(); x++)
        {
            std::string name = state.get_description().get_resolver().name(state.get_board()(x,y));
            int width=2;
            if(name == "empty")
            {
                s << "[" << std::setw(width) << " " << "] ";
            }
            else
            {
                s << "[" << std::setw(width) << name.substr(0, (unsigned long) width) << "] ";
            }
        }
        s << "\n";
    }
    return s;
}
