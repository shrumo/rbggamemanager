//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_ACTION_H
#define RBGGAMEMANAGER_ACTION_H

#include <unordered_set>
#include <memory>

#include "../game_components/name_resolver.h"
#include "condition.h"
#include "action_types.h"
#include "action_application.h"

class GameState;

// TODO(shrum): Add greedy actions and implement them in the search.
// Represents abstract action that can be called on a game state.
// Actions can be applied and reverted.
// Example:
//   Action& a = actions::Shift(2,2);
//   auto result = a.Apply(state);
class Action {
public:
  virtual ActionResult Apply(GameState *b) const =0;

  // Revert should be applied even if the action was not successful.
  virtual void Revert(GameState *, const ActionResult &) const {}

  bool IsModifier() const { return IsModifierType(type_); }

  bool IsSwitch() const { return IsSwitchType(type_); }

  ActionType type() const { return type_; }

  // Index is meaningful iff IsModifier() == true.
  unsigned int index() const { return index_; }

  virtual ~Action() = default;

protected:
  Action(ActionType type, unsigned int action_index)
      : type_(type), index_(action_index) {}

  explicit Action(ActionType type) : type_(type), index_(0) {}

private:
  ActionType type_;
  uint index_;
};

namespace actions {
  // Empty action does not change the game state.
  class Empty : public Action {
  public:
    Empty() : Action(ActionType::kEmptyType) {};

    ActionResult Apply(GameState *) const override { return true; };
  };

  // Shift changes the current position on board.
  class Shift : public Action {
  public:
    Shift(long long dx, long long dy) : Action(ActionType::kShiftType),
                                        delta_x_(dx),
                                        delta_y_(dy) {}

    ActionResult Apply(GameState *b) const override;

    void
    Revert(GameState *state, const ActionResult &apply_result) const override;

    // Returns the change in horizontal(x) axis.
    long long dx() const { return delta_x_; }

    // Returns the change in vertical(y) axis.
    long long dy() const { return delta_y_; }

  private:
    long long delta_x_, delta_y_;
  };

  // On checks if one of the pieces is on the current position on board.
  class On : public Action {
  public:
    // Parameter pieces should contain true for token ids of pieces names that the on accepts.
    explicit On(std::vector<bool> pieces)
        : Action(ActionType::kOnType), pieces_(std::move(pieces)) {}

    ActionResult Apply(GameState *b) const override;

    // Returns the pieces on requires on the board in order to be legal.
    const std::vector<bool> &pieces() const { return pieces_; }

  private:
    std::vector<bool> pieces_;
  };

  // Checks if some condition is true on the current state.
  class ConditionAction : public Action {
  public:
    explicit ConditionAction(std::unique_ptr<Condition> cond)
        : Action(ActionType::kConditionType), cond_(std::move(cond)) {}

    ActionResult Apply(GameState *b) const override { return cond_->Check(b); }

    const Condition *cond() const { return cond_.get(); }

  private:
    std::unique_ptr<Condition> cond_;
  };

  // Off places specified piece on the current position in game state.
  class Off : public Action {
  public:
    explicit Off(token_id_t piece, unsigned int index = 0) : Action(
        ActionType::kOffType, index), piece_(piece) {}

    ActionResult Apply(GameState *b) const override;

    void Revert(GameState *b, const ActionResult &apply_result) const override;

    token_id_t piece() const { return piece_; }

  private:
    token_id_t piece_;
  };

  // PlayerSwitch changes the player to the one specified.
  class PlayerSwitch : public Action {
  public:
    explicit PlayerSwitch(token_id_t player, unsigned int index = 0) : Action(
        ActionType::kSwitchType, index), player_(player) {}

    ActionResult Apply(GameState *b) const override;

    void Revert(GameState *b, const ActionResult &apply_result) const override;

    token_id_t player() const { return player_; }

  private:
    token_id_t player_;
  };

  // Assignment assigns a value to the variable.
  class Assignment : public Action {
  public:
    Assignment(size_t variable, std::unique_ptr<ArithmeticOperation> value,
               unsigned int index = 0)
        : Action(ActionType::kAssignmentType, index),
          variable(variable),
          value(std::move(value)) {}

    ActionResult Apply(GameState *b) const override;

    void Revert(GameState *b, const ActionResult &apply_result) const override;

    token_id_t get_variable() const { return variable; }

    const ArithmeticOperation *get_value_expression() const {
      return value.get();
    }

  private:
    token_id_t variable;
    std::unique_ptr<ArithmeticOperation> value;
  };

  // Increments a variable.
  class Incrementation : public Action {
  public:
    explicit Incrementation(size_t variable, unsigned int index = 0)
        : Action(ActionType::kIncrementationType, index), variable_(variable) {}

    ActionResult Apply(GameState *b) const override;

    void Revert(GameState *b, const ActionResult &apply_result) const override;

    token_id_t variable() const { return variable_; }

  private:
    token_id_t variable_;
  };

  // Decrements a variable.
  class Decrementation : public Action {
  public:
    explicit Decrementation(size_t variable, unsigned int index = 0)
        : Action(ActionType::kDecrementationType, index), variable_(variable) {}

    ActionResult Apply(GameState *b) const override;

    void Revert(GameState *b, const ActionResult &apply_result) const override;

    token_id_t variable() const { return variable_; }

  private:
    token_id_t variable_;
  };

  // Lazy actions are applied when modifier is applied.
  class Lazy : public Action {
  public:
    explicit Lazy(Action *lazy_action, unsigned int index = 0) : Action(
        ActionType::kLazyType, index), lazy_action_(lazy_action) {}

    ActionResult Apply(GameState *b) const override;

    void Revert(GameState *b, const ActionResult &apply_result) const override;

    const Action *action() const { return lazy_action_; }

  private:
    Action *lazy_action_;
  };
}


#endif //RBGGAMEMANAGER_ACTION_H
