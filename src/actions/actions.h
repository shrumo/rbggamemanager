#include <utility>

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
    explicit Action(ActionType type, uint id) : type_(type), id_(id) {}

    ActionType type() const { return type_; }

    uint id() const { return id_; }

    virtual std::string to_string() const {
      return ActionName(type_) + "_" + std::to_string(id_);
    }

  private:
    ActionType type_;
    uint id_;
  };

  class Empty : public Action {
  public:
    explicit Empty(uint id) : Action(ActionType::kEmptyType, id) {}
  };

  class Shift : public Action {
  public:
    explicit Shift(std::string edge, uint id) : Action(ActionType::kShiftType, id),
                                                edge_(std::move(edge)) {}

    const std::string &edge() const { return edge_; }

  private:
    std::string edge_;
  };

  class On : public Action {
  public:
    explicit On(std::vector<std::string> pieces, uint id) : Action(ActionType::kOnType, id),
                                                            pieces_(std::move(pieces)) {}

  private:
    std::vector<std::string> pieces_;
  };

  class Off : public Action {
  public:
    explicit Off(std::string piece, uint id) : Action(ActionType::kOffType, id),
                                               piece_(std::move(piece)) {}

    const std::string &piece() const { return piece_; }

  private:
    std::string piece_;
  };

  class Assignment : public Action {
  public:
    explicit Assignment(std::string variable, std::unique_ptr<ArithmeticOperation> value, uint id) : Action(
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

  class PlayerSwitch : public Action {
  public:
    explicit PlayerSwitch(std::string player_name, uint id) :
        Action(ActionType::kSwitchType, id), player_name_(std::move(player_name)) {}

    const std::string &player_name() const {
      return player_name_;
    }

  private:
    std::string player_name_;
  };

  class PlayerCheck : public Action {
  public:
    explicit PlayerCheck(std::string player, uint id) : Action(ActionType::kPlayerCheck, id),
                                                        player_(std::move(player)) {}

    const std::string &player() const { return player_; }

  private:
    std::string player_;
  };

  class ConditionCheck : public Action {
  public:

    ConditionCheck(std::unique_ptr<rbg::Graph<std::unique_ptr<Action>>> graph, node_t initial, node_t final,
                   ActionType check_type, uint id)
        : Action(check_type, id),
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

    node_t initial() const {
      return initial_;
    }

    node_t final() const {
      return final_;
    }

  private:
    std::unique_ptr<rbg::Graph<std::unique_ptr<Action>>> graph_;
    node_t initial_;
    node_t final_;
  };

  class ArithmeticComparison : public Action {
  public:
    ArithmeticComparison(std::unique_ptr<ArithmeticOperation> left,
                         std::unique_ptr<ArithmeticOperation> right, ActionType comparison_type, uint id)
        : Action(comparison_type, id),
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
