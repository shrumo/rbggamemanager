//
// Created by shrum on 28.05.19.
//

#include "arithmetic_operations_creator.h"
#include "parser_wrapper.h"
#include "parser_actions.h"

#include <actions/arithmetic.h>

using namespace rbg;

class ArithmeticOperationsCreator : public AstVisitorR<std::unique_ptr<ArithmeticOperation> > {
public:
  std::unique_ptr<ArithmeticOperation> defaultCase() override {
    throw std::logic_error("This is not an arithmetic expression.");
  }

  std::unique_ptr<ArithmeticOperation> visit(const rbg_parser::integer_arithmetic &expression) override {
    return std::make_unique<Constant>(expression.get_content());
  }

  std::unique_ptr<ArithmeticOperation> visit(const rbg_parser::variable_arithmetic &expression) override {
    return std::make_unique<Variable>(expression.get_content().to_string());
  }

  std::unique_ptr<ArithmeticOperation> visit(const rbg_parser::arithmetic_operation &expression) override {
    std::vector<std::unique_ptr<ArithmeticOperation>> elements;
    elements.reserve(expression.get_content().size());
    for (const auto &element : expression.get_content()) {
      elements.push_back(Fn<ArithmeticOperationsCreator, std::unique_ptr<ArithmeticOperation>>(*element));
    }
    switch (expression.get_operation()) {
      case rbg_parser::addition:
        return std::make_unique<Sum>(std::move(elements));
      case rbg_parser::subtraction:
        return std::make_unique<Subtraction>(std::move(elements));
      case rbg_parser::multiplication:
        return std::make_unique<Product>(std::move(elements));
      case rbg_parser::division:
        return std::make_unique<Division>(std::move(elements));
    }
  }
};

std::unique_ptr<rbg::ArithmeticOperation>
CreateArithmeticOperation(const rbg_parser::arithmetic_expression &expression) {
  return Fn<ArithmeticOperationsCreator, std::unique_ptr<rbg::ArithmeticOperation>>(expression);
}

