//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_CONDITION_H
#define RBGGAMEMANAGER_CONDITION_H

#include <vector>
#include <memory>
#include "arithmetic_operation.h"
#include "../game_nfa/automaton.h"
#include <limits>

class GameState;

class Action;

class Condition {
public:
  virtual bool Check(GameState *b) const =0;

  virtual ~Condition() = default;

  ConditionType type() const { return condition_type_; }

protected:
  explicit Condition(ConditionType type) : condition_type_(type) {};
private:
  ConditionType condition_type_;
};

namespace conditions {
  class Conjunction : public Condition {
  public:
    explicit Conjunction(std::vector<std::unique_ptr<Condition> > items)
        : Condition(ConditionType::kConjunctionType),
          items_(std::move(items)) {}

    bool Check(GameState *b) const override {
      for (auto &cond : items_) {
        if (!cond->Check(b))
          return false;
      }
      return true;
    }

    const std::vector<std::unique_ptr<Condition>> &items() const {
      return items_;
    }

  private:
    std::vector<std::unique_ptr<Condition> > items_;
  };

  class Alternative : public Condition {
  public:
    explicit Alternative(std::vector<std::unique_ptr<Condition> > items)
        : Condition(ConditionType::kAlternativeType),
          items_(std::move(items)) {}

    bool Check(GameState *b) const override {
      for (auto &cond : items_) {
        if (cond->Check(b))
          return true;
      }
      return false;
    }

    const std::vector<std::unique_ptr<Condition>> &
    items() const { return items_; }

  private:
    std::vector<std::unique_ptr<Condition> > items_;
  };

  class Negation : public Condition {
  public:
    explicit Negation(std::unique_ptr<Condition> item)
        : Condition(ConditionType::kNegationType),
          item_(std::move(item)) {}

    bool Check(GameState *b) const override { return !item_->Check(b); }

    const Condition *condition() const { return item_.get(); }

  private:
    std::unique_ptr<Condition> item_;
  };

  class Less : public Condition {
  public:
    Less(std::unique_ptr<ArithmeticOperation> left,
         std::unique_ptr<ArithmeticOperation> right)
        : Condition(ConditionType::kLessType),
          left_(std::move(left)),
          right_(std::move(right)) {}

    bool Check(GameState *b) const override {
      return left_->Value(b) < right_->Value(b);
    }

    const ArithmeticOperation *left() const {
      return left_.get();
    }

    const ArithmeticOperation *right() const {
      return right_.get();
    }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };

  class LessEqual : public Condition {
  public:
    LessEqual(std::unique_ptr<ArithmeticOperation> left,
              std::unique_ptr<ArithmeticOperation> right)
        : Condition(ConditionType::kLessEqualType),
          left_(std::move(left)),
          right_(std::move(right)) {}

    bool Check(GameState *b) const override {
      return left_->Value(b) <= right_->Value(b);
    }

    const ArithmeticOperation *left() const {
      return left_.get();
    }

    const ArithmeticOperation *right() const {
      return right_.get();
    }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };

  class Equal : public Condition {
  public:
    Equal(std::unique_ptr<ArithmeticOperation> left,
          std::unique_ptr<ArithmeticOperation> right)
        : Condition(ConditionType::kEqualType),
          left_(std::move(left)),
          right_(std::move(right)) {}

    bool Check(GameState *b) const override {
      return left_->Value(b) == right_->Value(b);
    }

    const ArithmeticOperation *left() const {
      return left_.get();
    }

    const ArithmeticOperation *right() const {
      return right_.get();
    }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };

  class NotEqual : public Condition {
  public:
    NotEqual(std::unique_ptr<ArithmeticOperation> left,
             std::unique_ptr<ArithmeticOperation> right)
        : Condition(ConditionType::kNotEqualType),
          left_(std::move(left)),
          right_(std::move(right)) {}

    bool Check(GameState *b) const override {
      return left_->Value(b) != right_->Value(b);
    }

    const ArithmeticOperation *left() const {
      return left_.get();
    }

    const ArithmeticOperation *right() const {
      return right_.get();
    }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };

  // Checks whether there exists a move starting 
  // from the game state in specified nfa.
  class MovePattern : public Condition {
  public:
    MovePattern(unsigned int index,
                std::unique_ptr<fsm::Nfa<const Action *> > move_nfa)
        : Condition(ConditionType::kMovePatternType),
          index_(index),
          move_nfa_(std::move(move_nfa)) {}

    bool Check(GameState *b) const override;

    const fsm::Nfa<const Action *> *nfa() const {
      return move_nfa_.get();
    }

  private:
    unsigned int index_;
    std::unique_ptr<fsm::Nfa<const Action *> > move_nfa_;
  };
}


#endif //RBGGAMEMANAGER_CONDITION_H
