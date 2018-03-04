//
// Created by shrum on 12.01.18.
//

#include "arithmetic_operation.h"
#include "../game_components/game_state.h"

int arithmetic_operations::Variable::Value(GameState *b) const {
  return (b->Value(variable_) + b->description().bound()) %
         b->description().bound();
}

int arithmetic_operations::Constant::Value(GameState *b) const {
  return (constant_ + b->description().bound()) % b->description().bound();
}
