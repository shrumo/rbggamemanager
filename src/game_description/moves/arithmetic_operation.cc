//
// Created by shrum on 27.06.19.
//

#include "arithmetic_operation.h"

#include "game_state/game_state.h"

using namespace rbg;

variable_value_t VariableValue::Value(const GameState &s) const {
  return s.variables_values_[variable_id_];
}

variable_value_t ConstantValue::Value(const GameState &) const {
  return value_;
}

variable_value_t SumValue::Value(const GameState &s) const {
  variable_value_t result = summands_[0]->Value(s);
  for (size_t i = 1; i < summands_.size(); i++) {
    result += summands_[i]->Value(s);
  }
  return result;
}

variable_value_t SubtractionValue::Value(const GameState &s) const {
  variable_value_t result = elements_[0]->Value(s);
  for (size_t i = 1; i < elements_.size(); i++) {
    result -= elements_[i]->Value(s);
  }
  return result;
}

variable_value_t ProductValue::Value(const GameState &s) const {
  variable_value_t result = factors_[0]->Value(s);
  for (size_t i = 1; i < factors_.size(); i++) {
    result *= factors_[i]->Value(s);
  }
  return result;
}

variable_value_t DivisionValue::Value(const GameState &s) const {
  variable_value_t result = elements_[0]->Value(s);
  for (size_t i = 1; i < elements_.size(); i++) {
    result /= elements_[i]->Value(s);
  }
  return result;
}

variable_value_t PieceCountValue::Value(const GameState &s) const {
  return s.board_.piece_count(piece_);
}
