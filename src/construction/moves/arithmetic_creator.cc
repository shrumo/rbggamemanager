//
// Created by shrum on 17.06.19.
//

#include "arithmetic_creator.h"

using namespace rbg;
using namespace std;

class ArithmeticCreator : public AstFunction<unique_ptr<rbg::ArithmeticOperation>> {
public:
  explicit ArithmeticCreator(const Declarations &declarations)
      : declarations_(declarations) {}

  unique_ptr<ArithmeticOperation>
  IntegerArithmeticCase(const rbg_parser::integer_arithmetic &expression) override {
    return make_unique<ConstantValue>(expression.get_content());
  }

  unique_ptr<ArithmeticOperation>
  VariableArithmeticCase(const rbg_parser::variable_arithmetic &expression) override {
    return make_unique<VariableValue>(declarations_.variables_resolver.Id(expression.get_content().to_string()));
  }

  unique_ptr<ArithmeticOperation>
  ArithmeticOperationCase(const rbg_parser::arithmetic_operation &expression) override {
    vector<unique_ptr<ArithmeticOperation>> elements;
    for (const auto &subexpr : expression.get_content()) {
      elements.push_back(ArithmeticCreator(declarations_)(*subexpr));
    }
    switch (expression.get_operation()) {
      case rbg_parser::addition:
        return make_unique<SumValue>(std::move(elements));
      case rbg_parser::subtraction:
        return make_unique<SubtractionValue>(std::move(elements));
      case rbg_parser::multiplication:
        return make_unique<ProductValue>(std::move(elements));
      case rbg_parser::division:
        return make_unique<DivisionValue>(std::move(elements));
    }
    return nullptr;
  }

private:
  const Declarations &declarations_;
};

unique_ptr<rbg::ArithmeticOperation>
rbg::CreateArithmeticOperation(const rbg_parser::arithmetic_expression &expression, const Declarations &declarations) {
  return ArithmeticCreator(declarations)(expression);
}