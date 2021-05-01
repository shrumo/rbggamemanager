//
// Created by shrum on 17.06.19.
//

#ifndef RBGGAMEMANAGER_ARITHMETIC_OPERATION_VISITOR_H
#define RBGGAMEMANAGER_ARITHMETIC_OPERATION_VISITOR_H

#include "game_description/moves/arithmetic_operation.h"

namespace rbg {
template <typename ResultType>
class ArithmeticOperationFunction : private ArithmeticOperationVisitor {
public:
  virtual ResultType VariableCase(const VariableValue &variable_value) {
    return DefaultCase(variable_value);
  }

  virtual ResultType ConstantCase(const ConstantValue &constant_value) {
    return DefaultCase(constant_value);
  }

  virtual ResultType SumCase(const SumValue &sum_value) {
    return DefaultCase(sum_value);
  }

  virtual ResultType SubtractionCase(const SubtractionValue &subtaction_value) {
    return DefaultCase(subtaction_value);
  }

  virtual ResultType ProductCase(const ProductValue &product_value) {
    return DefaultCase(product_value);
  }

  virtual ResultType DivisionCase(const DivisionValue &division_value) {
    return DefaultCase(division_value);
  }

  virtual ResultType PieceCountCase(const PieceCountValue &piececount_value) {
    return DefaultCase(piececount_value);
  }

  virtual ResultType DefaultCase(const ArithmeticOperation &) {
    return ResultType();
  }

  template <typename NodeType>
  ResultType operator()(const NodeType &operation) {
    operation.Accept(*this);
    assert(result_exists_ &&
           "You cannot reuse the result. Extraction of the result can happen "
           "exactly once after calling Visit.");
    result_exists_ = false;
    return std::move(result_);
  }

private:
  void Visit(const VariableValue &m) override {
    result_ = VariableCase(m);
    result_exists_ = true;
  }

  void Visit(const ConstantValue &m) override {
    result_ = ConstantCase(m);
    result_exists_ = true;
  }

  void Visit(const SumValue &m) override {
    result_ = SumCase(m);
    result_exists_ = true;
  }

  void Visit(const SubtractionValue &m) override {
    result_ = SubtractionCase(m);
    result_exists_ = true;
  }

  void Visit(const ProductValue &m) override {
    result_ = ProductCase(m);
    result_exists_ = true;
  }

  void Visit(const DivisionValue &m) override {
    result_ = DivisionCase(m);
    result_exists_ = true;
  }

  void Visit(const PieceCountValue &m) override {
    result_ = PieceCountCase(m);
    result_exists_ = true;
  }

  bool result_exists_ = false;
  ResultType result_;
};
} // namespace rbg

#endif // RBGGAMEMANAGER_ARITHMETIC_OPERATION_VISITOR_H
