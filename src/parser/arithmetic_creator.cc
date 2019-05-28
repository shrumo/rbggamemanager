//
// Created by shrum on 07.05.18.
//

#include "arithmetic_creator.h"
#include "parser_wrapper.h"

class ArithmeticCreator : public AstVisitorR<std::unique_ptr<ArithmeticOperation> > {
public:
  explicit ArithmeticCreator(const NameResolver &resolver)
      : resolver_(resolver) {}

private:
  std::unique_ptr<ArithmeticOperation> visit(const rbg_parser::integer_arithmetic &arithmetic) override;

  std::unique_ptr<ArithmeticOperation> visit(const rbg_parser::variable_arithmetic &arithmetic) override;

  std::unique_ptr<ArithmeticOperation> visit(const rbg_parser::arithmetic_operation &arithmetic) override;

  const NameResolver &resolver_;
};

std::unique_ptr<ArithmeticOperation>
CreateOperation(const rbg_parser::arithmetic_expression &expression, const NameResolver &resolver) {
  return Fn<ArithmeticCreator, std::unique_ptr<ArithmeticOperation> >(expression, resolver);
}

std::unique_ptr<ArithmeticOperation> ArithmeticCreator::visit(const rbg_parser::integer_arithmetic &arithmetic) {
  return std::unique_ptr<ArithmeticOperation>(new arithmetic_operations::Constant(arithmetic.get_content()));
}

std::unique_ptr<ArithmeticOperation> ArithmeticCreator::visit(const rbg_parser::variable_arithmetic &arithmetic) {
  token_id_t variable_id = resolver_.Id(arithmetic.get_content().to_string());
  return std::unique_ptr<ArithmeticOperation>(new arithmetic_operations::Variable(variable_id));
}

std::unique_ptr<ArithmeticOperation> ArithmeticCreator::visit(const rbg_parser::arithmetic_operation &arithmetic) {
  std::vector<std::unique_ptr<ArithmeticOperation> > operations;
  for (const auto &child : arithmetic.get_content()) {
    operations.push_back(CreateOperation(*child, resolver_));
  }
  switch (arithmetic.get_operation()) {
    case rbg_parser::addition:
      return std::unique_ptr<ArithmeticOperation>(new arithmetic_operations::Sum(std::move(operations)));

    case rbg_parser::subtraction:
      return std::unique_ptr<ArithmeticOperation>(new arithmetic_operations::Subtraction(std::move(operations)));

    case rbg_parser::multiplication:
      return std::unique_ptr<ArithmeticOperation>(new arithmetic_operations::Product(std::move(operations)));

    case rbg_parser::division:
      return std::unique_ptr<ArithmeticOperation>(new arithmetic_operations::Division(std::move(operations)));
  }
  return std::unique_ptr<ArithmeticOperation>();
}

