//
// Created by shrum on 12.01.18.
//

#include "arithmetic_operation.h"
#include "../game_components/game_state.h"

int arithmetic_operations::variable_value::value(game_state *b) const {
    return b->value(variable);
}
