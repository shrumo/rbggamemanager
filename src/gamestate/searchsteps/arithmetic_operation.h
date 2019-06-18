//
// Created by shrum on 17.06.19.
//

#ifndef RBGGAMEMANAGER_ARITHMETIC_OPERATION_H
#define RBGGAMEMANAGER_ARITHMETIC_OPERATION_H

#include "../gamestate.h"
#include <vector>

namespace rbg {

  class VariableValue;

  class ConstantValue;

  class SumValue;

  class SubtractionValue;

  class ProductValue;

  class DivisionValue;

  class PieceCountValue;

  class ArithmeticOperationVisitor {
  public:
    virtual void Visit(const VariableValue &) = 0;

    virtual void Visit(const ConstantValue &) = 0;

    virtual void Visit(const SumValue &) = 0;

    virtual void Visit(const SubtractionValue &) = 0;

    virtual void Visit(const ProductValue &) = 0;

    virtual void Visit(const DivisionValue &) = 0;

    virtual void Visit(const PieceCountValue &) = 0;
  };

  class ArithmeticOperation {
  public:
    virtual variable_value_t Value(const GameState &s) const = 0;

    virtual void Accept(ArithmeticOperationVisitor &visitor) const = 0;
  };

  class VariableValue : public ArithmeticOperation {
  public:
    explicit VariableValue(variable_id_t variable_id) : variable_id_(variable_id) {}

    variable_value_t Value(const GameState &s) const override {
      return s.variables_values_[variable_id_];
    }


    void Accept(ArithmeticOperationVisitor &visitor) const override {
      visitor.Visit(*this);
    }

    variable_id_t variable_id() const { return variable_id_; }

  private:
    variable_id_t variable_id_;
  };

  class ConstantValue : public ArithmeticOperation {
  public:
    explicit ConstantValue(variable_value_t value) : value_(value) {}

    variable_value_t Value(const GameState &) const override {
      return value_;
    }

    void Accept(ArithmeticOperationVisitor &visitor) const override {
      visitor.Visit(*this);
    }

    variable_value_t value() const { return value_; }

  private:
    variable_value_t value_;
  };

  class SumValue : public ArithmeticOperation {
  public:
    explicit SumValue(std::vector<std::unique_ptr<ArithmeticOperation>> summands) : summands_(std::move(summands)) {}

    variable_value_t Value(const GameState &s) const override {
      variable_value_t result = summands_[0]->Value(s);
      for (size_t i = 1; i < summands_.size(); i++) {
        result += summands_[i]->Value(s);
      }
      return result;
    }

    void Accept(ArithmeticOperationVisitor &visitor) const override {
      visitor.Visit(*this);
    }

    const std::vector<std::unique_ptr<ArithmeticOperation>> &summands() const {
      return summands_;
    }

  private:
    std::vector<std::unique_ptr<ArithmeticOperation>> summands_;
  };

  class SubtractionValue : public ArithmeticOperation {
  public:
    explicit SubtractionValue(std::vector<std::unique_ptr<ArithmeticOperation>> elements) : elements_(
        std::move(elements)) {}

    variable_value_t Value(const GameState &s) const override {
      variable_value_t result = elements_[0]->Value(s);
      for (size_t i = 1; i < elements_.size(); i++) {
        result -= elements_[i]->Value(s);
      }
      return result;
    }

    void Accept(ArithmeticOperationVisitor &visitor) const override {
      visitor.Visit(*this);
    }


    const std::vector<std::unique_ptr<ArithmeticOperation>> &elements() const {
      return elements_;
    }

  private:
    std::vector<std::unique_ptr<ArithmeticOperation>> elements_;
  };

  class ProductValue : public ArithmeticOperation {
  public:
    explicit ProductValue(std::vector<std::unique_ptr<ArithmeticOperation>> factors) : factors_(std::move(factors)) {}

    variable_value_t Value(const GameState &s) const override {
      variable_value_t result = factors_[0]->Value(s);
      for (size_t i = 1; i < factors_.size(); i++) {
        result *= factors_[i]->Value(s);
      }
      return result;
    }

    void Accept(ArithmeticOperationVisitor &visitor) const override {
      visitor.Visit(*this);
    }


    const std::vector<std::unique_ptr<ArithmeticOperation>> &factors() const {
      return factors_;
    }

  private:
    std::vector<std::unique_ptr<ArithmeticOperation>> factors_;
  };

  class DivisionValue : public ArithmeticOperation {
  public:
    explicit DivisionValue(std::vector<std::unique_ptr<ArithmeticOperation>> elements) : elements_(
        std::move(elements)) {}

    variable_value_t Value(const GameState &s) const override {
      variable_value_t result = elements_[0]->Value(s);
      for (size_t i = 1; i < elements_.size(); i++) {
        result /= elements_[i]->Value(s);
      }
      return result;
    }

    void Accept(ArithmeticOperationVisitor &visitor) const override {
      visitor.Visit(*this);
    }

    const std::vector<std::unique_ptr<ArithmeticOperation>> &elements() const {
      return elements_;
    }

  private:
    std::vector<std::unique_ptr<ArithmeticOperation>> elements_;
  };

  class PieceCountValue : public ArithmeticOperation {
  public:
    explicit PieceCountValue(piece_id_t piece) : piece_(piece) {}

    variable_value_t Value(const GameState &s) const override {
      return s.board_.piece_count(piece_);
    }

    void Accept(ArithmeticOperationVisitor &visitor) const override {
      visitor.Visit(*this);
    }

    piece_id_t piece() const { return piece_; }

  private:
    piece_id_t piece_;
  };
}

#endif //RBGGAMEMANAGER_ARITHMETIC_OPERATION_H
