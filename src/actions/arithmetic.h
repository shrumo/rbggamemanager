#include <utility>

//
// Created by shrum on 28.05.19.
//

#ifndef RBGGAMEMANAGER_ARITHMETIC_H
#define RBGGAMEMANAGER_ARITHMETIC_H

#include "types.h"
#include <string>
#include <vector>
#include <memory>

namespace rbg {
  class ArithmeticOperation {
  public:
    ArithmeticOperationType type() const { return type_; }

  protected:
    explicit ArithmeticOperation(ArithmeticOperationType type) : type_(type) {}

  private:
    ArithmeticOperationType type_;
  };

  class Variable : public ArithmeticOperation {
  public:
    explicit Variable(std::string variable_name)
        : ArithmeticOperation(ArithmeticOperationType::kVariableType), name_(std::move(variable_name)) {}

    const std::string &name() const { return name_; }

  private:
    std::string name_;
  };

  class Constant : public ArithmeticOperation {
  public:
    explicit Constant(uint value)
        : ArithmeticOperation(ArithmeticOperationType::kConstantType), value_(value) {}

    uint value() const { return value_; }

  private:
    uint value_;
  };

  class Sum : public ArithmeticOperation {
  public:
    explicit Sum(std::vector<std::unique_ptr<ArithmeticOperation> > summands) :
        ArithmeticOperation(ArithmeticOperationType::kSumType), summands_(std::move(summands)) {}

    const std::vector<std::unique_ptr<ArithmeticOperation> > &summands() const {
      return summands_;
    }

  private:
    std::vector<std::unique_ptr<ArithmeticOperation> > summands_;
  };

  class Subtraction : public ArithmeticOperation {
  public:
    explicit Subtraction(std::vector<std::unique_ptr<ArithmeticOperation> > elements) :
        ArithmeticOperation(ArithmeticOperationType::kSubtractionType), elements_(std::move(elements)) {}

    const std::vector<std::unique_ptr<ArithmeticOperation> > &elements() const {
      return elements_;
    }

  private:
    std::vector<std::unique_ptr<ArithmeticOperation> > elements_;
  };


  class Product : public ArithmeticOperation {
  public:
    explicit Product(std::vector<std::unique_ptr<ArithmeticOperation> > factors) :
        ArithmeticOperation(ArithmeticOperationType::kProductType), factors_(std::move(factors)) {}

    const std::vector<std::unique_ptr<ArithmeticOperation> > &factors() const {
      return factors_;
    }

  private:
    std::vector<std::unique_ptr<ArithmeticOperation> > factors_;
  };


  class Division : public ArithmeticOperation {
  public:
    explicit Division(std::vector<std::unique_ptr<ArithmeticOperation> > elements) :
        ArithmeticOperation(ArithmeticOperationType::kDivisionType), elements_(std::move(elements)) {}

    const std::vector<std::unique_ptr<ArithmeticOperation> > &elements() const {
      return elements_;
    }

  private:
    std::vector<std::unique_ptr<ArithmeticOperation> > elements_;
  };
}


#endif //RBGGAMEMANAGER_ARITHMETIC_H
