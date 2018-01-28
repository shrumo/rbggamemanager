//
// Created by shrum on 12.01.18.
//

#include "condition.h"
#include "../game_components/game_state.h"

bool conditions::move_pattern::check(game_state *b) const {
    return b->get_search_context().check_pattern(*move_nfa, index);
}
