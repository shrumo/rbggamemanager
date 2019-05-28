#include <utility>

//
// Created by shrum on 28.05.19.
//

#ifndef RBGGAMEMANAGER_ACTIONS_H
#define RBGGAMEMANAGER_ACTIONS_H

#include "types.h"
#include "arithmetic.h"
#include <string>
#include <vector>
#include <game_nfa/graph.h>

namespace rbg {
  class Action {
  public:
    virtual ~Action() = default;

  protected:
    explicit Action(ActionType type) : type_(type) {}

    ActionType type() const { return type_; }

  private:
    ActionType type_;
  };

  class Modifier : public Action {
  protected:
    Modifier(ActionType type, uint id) : Action(type), id_(id) {}

    uint id() const { return id_; }

  private:
    uint id_;
  };

  class Empty : public Action {
  public:
    Empty() : Action(ActionType::kEmptyType) {}
  };

  class Shift : public Action {
  public:
    explicit Shift(std::string edge) : Action(ActionType::kShiftType),
                                       edge_(std::move(edge)) {}

    const std::string &edge() const { return edge_; }

  private:
    std::string edge_;
  };

  class On : public Action {
  public:
    explicit On(std::vector<std::string> pieces) : Action(ActionType::kOnType), pieces_(std::move(pieces)) {}

  private:
    std::vector<std::string> pieces_;
  };

  class Off : public Modifier {
  public:
    explicit Off(std::string piece, uint id) : Modifier(ActionType::kOffType, id),
                                               piece_(std::move(piece)) {}

    const std::string &piece() const { return piece_; }

  private:
    std::string piece_;
  };

  class Assignment : Modifier {
  public:
    explicit Assignment(std::string variable, std::unique_ptr<ArithmeticOperation> value, uint id) : Modifier(
        ActionType::kAssignmentType, id),
                                                                                                     variable_(
                                                                                                         std::move(
                                                                                                             variable)),
                                                                                                     value_(std::move(
                                                                                                         value)) {}

    const std::string &variable() const { return variable_; }

    const ArithmeticOperation *value() const {
      return value_.get();
    }

  private:
    std::string variable_;
    std::unique_ptr<ArithmeticOperation> value_;
  };

  class PlayerCheck : Action {
  public:
    explicit PlayerCheck(std::string player) : Action(ActionType::kPlayerCheck), player_(std::move(player)) {}

    const std::string &player() const { return player_; }

  private:
    std::string player_;
  };

  class ConditionCheck : public Action {
  public:

    ConditionCheck(std::unique_ptr<rbg::Graph<std::unique_ptr<Action>>> graph, node_t initial, node_t final,
                   ActionType check_type)
        : Action(check_type),
          graph_(std::move(graph)),
          initial_(initial),
          final_(final) {
      if (check_type != ActionType::kConditionCheckType && check_type != ActionType::kNegatedConditionCheckType) {
        throw std::logic_error(
            "Condition check should have either type kConditionCheckType or kNegatedConditionCheckType.");
      }
    }

    const rbg::Graph<std::unique_ptr<Action>> *graph() const {
      return graph_.get();
    }

  private:
    std::unique_ptr<rbg::Graph<std::unique_ptr<Action>>> graph_;
    node_t initial_;
    node_t final_;
  };

  class ArithmeticComparison : public Action {
  public:
    ArithmeticComparison(std::unique_ptr<ArithmeticOperation> left,
                         std::unique_ptr<ArithmeticOperation> right, ActionType comparison_type)
        : Action(comparison_type),
          left_(std::move(left)),
          right_(std::move(right)) {}

    const ArithmeticOperation *left() const {
      return left_.get();
    }

    const ArithmeticOperation *right() const {
      return right_.get();
    }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };
}


#endif //RBGGAMEMANAGER_ACTIONS_H
