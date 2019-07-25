//
// Created by shrum on 17.06.19.
//

#include "arithmetic_creator.h"
#include <utility/arithmetic_operation_visitor.h>

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
    if (declarations_.pieces_resolver.contains(expression.get_content().to_string())) {
      return make_unique<PieceCountValue>(declarations_.pieces_resolver.Id(expression.get_content().to_string()));
    }
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

class ArithmeticCopyCreator : public ArithmeticOperationFunction<std::unique_ptr<ArithmeticOperation>> {
public:
  unique_ptr<ArithmeticOperation> VariableCase(const VariableValue &variable_value) override {
    return make_unique<VariableValue>(variable_value.variable_id());
  }

  unique_ptr<ArithmeticOperation> ConstantCase(const ConstantValue &constant_value) override {
    return make_unique<ConstantValue>(constant_value.value());
  }

  unique_ptr<ArithmeticOperation> SumCase(const SumValue &sum_value) override {
    vector<unique_ptr<ArithmeticOperation>> new_children;
    for (const auto &child : sum_value.summands()) {
      new_children.push_back(ArithmeticCopyCreator()(*child));
    }
    return make_unique<SumValue>(std::move(new_children));
  }

  unique_ptr<ArithmeticOperation> SubtractionCase(const SubtractionValue &subtraction_value) override {
    vector<unique_ptr<ArithmeticOperation>> new_children;
    for (const auto &child : subtraction_value.elements()) {
      new_children.push_back(ArithmeticCopyCreator()(*child));
    }
    return make_unique<SubtractionValue>(std::move(new_children));
  }

  unique_ptr<ArithmeticOperation> ProductCase(const ProductValue &product_value) override {
    vector<unique_ptr<ArithmeticOperation>> new_children;
    for (const auto &child : product_value.factors()) {
      new_children.push_back(ArithmeticCopyCreator()(*child));
    }
    return make_unique<ProductValue>(std::move(new_children));
  }

  unique_ptr<ArithmeticOperation> DivisionCase(const DivisionValue &division_value) override {
    vector<unique_ptr<ArithmeticOperation>> new_children;
    for (const auto &child : division_value.elements()) {
      new_children.push_back(ArithmeticCopyCreator()(*child));
    }
    return make_unique<DivisionValue>(std::move(new_children));
  }

  unique_ptr<ArithmeticOperation> PieceCountCase(const PieceCountValue &piececount_value) override {
    return make_unique<PieceCountValue>(piececount_value.piece());
  }

};

unique_ptr<rbg::ArithmeticOperation>
rbg::CreateArithmeticOperation(const rbg_parser::arithmetic_expression &expression, const Declarations &declarations) {
  return ArithmeticCreator(declarations)(expression);
}

std::unique_ptr<rbg::ArithmeticOperation>
rbg::CreateArithmeticOperation(const ArithmeticOperation &arithmeticOperation) {
  return ArithmeticCopyCreator()(arithmeticOperation);
}