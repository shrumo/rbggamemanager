//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_ARITHMETIC_OPERATION_H
#define RBGGAMEMANAGER_ARITHMETIC_OPERATION_H

#include "../game_components/name_resolver.h"
#include "action_types.h"

class GameState;

// Used to represent all possible arithmetic operations that
// can be created from states variables and constant values.
// Example:
//   ArithmeticOperation& ao = arithmetic_operations::variable(17);
//   int value = ao.Value(state);
//   if (value == state.Value(17))
class ArithmeticOperation {
public:
  virtual int Value(GameState *b) const =0;

  virtual ~ArithmeticOperation() = default;

  ArithmeticOperationType type() const { return type_; }

protected:
  explicit ArithmeticOperation(ArithmeticOperationType type) : type_(type) {}

private:
  ArithmeticOperationType type_;
};

namespace arithmetic_operations {
  // Gives the value of the variable in current state.
  class Variable : public ArithmeticOperation {
  public:
    explicit Variable(token_id_t variable) :
        ArithmeticOperation(ArithmeticOperationType::kVariableType),
        variable_(variable) {}

    int Value(GameState *b) const override;

    token_id_t variable() const {
      return variable_;
    }

  private:
    token_id_t variable_;
  };

  class Constant : public ArithmeticOperation {
  public:
    explicit Constant(int constant) :
        ArithmeticOperation(ArithmeticOperationType::kConstantType),
        constant_(constant) {}

    int Value(GameState *b) const override;

    int Value() const { return Value(nullptr); };

    int constant() {
      return constant_;
    }

  private:
    int constant_;
  };
}


#endif //RBGGAMEMANAGER_ARITHMETIC_OPERATION_H
