//
// Created by shrum on 12/07/19.
//

#ifndef RBGGAMEMANAGER_ACTIONS_H
#define RBGGAMEMANAGER_ACTIONS_H

#include <game_description/declarations.h>
#include <game_state/game_state.h>

#include <vector>

#include "game_description/moves/arithmetic_operation.h"
#include "modifying_application.h"
#include "utility/resetabble_bitarray_stack.h"

namespace rbg {
class GameState;

class BlocksCollection;

class SearchStepsPoint;
}  // namespace rbg

namespace rbg {

class VisitedCheckTest {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kCheck;

  explicit VisitedCheckTest(ResetableBitArrayStackChunk bit_array_stack_chunk)
      : stack_chunk_(bit_array_stack_chunk) {}

  inline bool Check(GameState *state) {
    bool result = stack_chunk_[state->current_pos_];
    stack_chunk_.set(state->current_pos_);
    return !result;
  }

 private:
  ResetableBitArrayStackChunk stack_chunk_;
};

class ConditionVisitedCheckTest {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kSpecial;

  explicit ConditionVisitedCheckTest(
      ResetableBitArrayStackChunk bit_array_stack_chunk,
      ResetableBitArrayStackChunk results_chunk)
      : stack_chunk_(bit_array_stack_chunk), results_chunk_(results_chunk) {}

  template <typename NextBlockElements>
  inline void Run(GameState *, NextBlockElements *) {
    assert(false && "This shouldn't be run.");
  }

  template <typename NextBlockElements>
  inline bool RunAndApplyFirst(GameState *, std::vector<ActionRevertInfo> *,
                               vertex_id_t, NextBlockElements *) {
    assert(false && "This shouldn't be run.");
  }

  template <typename NextBlockElements>
  inline bool RunAndFindEnd(GameState *state, NextBlockElements *next) {
    bool visited = stack_chunk_[state->current_pos_];
    if (visited || results_chunk_[state->current_pos_]) {
      return results_chunk_[state->current_pos_];
    }
    stack_chunk_.set(state->current_pos_);
    bool result = next->RunAndFindEnd(state);
    if (result) {
      results_chunk_.set(state->current_pos_);
    }
    return result;
  }

 private:
  ResetableBitArrayStackChunk stack_chunk_;
  ResetableBitArrayStackChunk results_chunk_;
};

class ShiftAction {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kApplication;
  using revert_info_t = vertex_id_t;

  explicit ShiftAction(shift_edge_id edge_id) : edge_id_(edge_id) {}

  inline revert_info_t Apply(GameState *state) const {
    vertex_id_t previous_vertex = state->current_pos_;
    state->current_pos_ = state->declarations().initial_board().NextVertex(
        state->current_pos_, edge_id_);
    return previous_vertex;
  }

  inline void Revert(GameState *state, revert_info_t previous_vertex) const {
    state->current_pos_ = previous_vertex;
  }

 private:
  shift_edge_id edge_id_;
};

class OutOfBoundsTest {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kCheck;

  inline bool Check(GameState *state) const {
    return state->current_pos() <
           state->declarations().initial_board().vertices_count();
  }
};

class OnTest {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kCheck;

  explicit OnTest(std::vector<bool> pieces) : pieces_(std::move(pieces)) {}

  inline bool Check(GameState *state) const {
    return pieces_[state->board_.at(state->current_pos_)];
  }

 private:
  std::vector<bool> pieces_;
};

class PlayerTest {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kCheck;

  explicit PlayerTest(player_id_t player) : player_(player) {}

  inline bool Check(GameState *state) const {
    return state->current_player_ == player_;
  }

 private:
  player_id_t player_;
};

class LessEqualComparisonTest {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kCheck;

  explicit LessEqualComparisonTest(std::unique_ptr<ArithmeticOperation> left,
                                   std::unique_ptr<ArithmeticOperation> right)
      : left_(std::move(left)), right_(std::move(right)) {}

  inline bool Check(GameState *state) const {
    return left_->Value(*state) <= right_->Value(*state);
  }

 private:
  std::unique_ptr<ArithmeticOperation> left_, right_;
};

class LessComparisonTest {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kCheck;

  explicit LessComparisonTest(std::unique_ptr<ArithmeticOperation> left,
                              std::unique_ptr<ArithmeticOperation> right)
      : left_(std::move(left)), right_(std::move(right)) {}

  inline bool Check(GameState *state) const {
    return left_->Value(*state) < right_->Value(*state);
  }

 private:
  std::unique_ptr<ArithmeticOperation> left_, right_;
};

class EqualComparisonTest {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kCheck;

  explicit EqualComparisonTest(std::unique_ptr<ArithmeticOperation> left,
                               std::unique_ptr<ArithmeticOperation> right)
      : left_(std::move(left)), right_(std::move(right)) {}

  inline bool Check(GameState *state) const {
    return left_->Value(*state) == right_->Value(*state);
  }

 private:
  std::unique_ptr<ArithmeticOperation> left_, right_;
};

class NotEqualComparisonTest {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kCheck;

  explicit NotEqualComparisonTest(std::unique_ptr<ArithmeticOperation> left,
                                  std::unique_ptr<ArithmeticOperation> right)
      : left_(std::move(left)), right_(std::move(right)) {}

  inline bool Check(GameState *state) const {
    return left_->Value(*state) != right_->Value(*state);
  }

 private:
  std::unique_ptr<ArithmeticOperation> left_, right_;
};

class ConditionCheckTest {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kCheck;

  explicit ConditionCheckTest(Block *steps_point)
      : steps_point_(std::move(steps_point)){};

  inline bool Check(GameState *state) const {
    state->blocks_.PushVisitedStack();
    bool result = steps_point_->RunAndFindEnd(state);
    state->blocks_.PopVisitedStack();
    return result;
  }

 private:
  Block *steps_point_;
};

class NegatedConditionCheckTest {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kCheck;

  explicit NegatedConditionCheckTest(Block *steps_point)
      : steps_point_(std::move(steps_point)){};

  inline bool Check(GameState *state) const {
    state->blocks_.PushVisitedStack();
    bool result = !steps_point_->RunAndFindEnd(state);
    state->blocks_.PopVisitedStack();
    return result;
  }

 private:
  Block *steps_point_;
};

class OffApplication : public ModifyingApplication {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kModifier;
  using revert_info_t = piece_id_t;

  explicit OffApplication(revert_info_t piece_id, uint index)
      : ModifyingApplication(index), piece_id_(piece_id) {}

  revert_info_t Apply(GameState *state) const override {
    piece_id_t previous_piece = state->board_.at(state->current_pos_);
    state->board_.set(state->current_pos_, piece_id_);
    return previous_piece;
  }

  void Revert(GameState *state, revert_info_t previous_piece) const override {
    state->board_.set(state->current_pos_, previous_piece);
  }

 private:
  piece_id_t piece_id_;
};

class PlayerSwitchApplication : public ModifyingApplication {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kSwitch;
  using revert_info_t = player_id_t;

  explicit PlayerSwitchApplication(player_id_t player_id, uint index)
      : ModifyingApplication(index), player_id_(player_id) {}

  revert_info_t Apply(GameState *state) const override {
    player_id_t previous_player = state->current_player_;
    state->current_player_ = player_id_;
    return previous_player;
  }

  void Revert(GameState *state, revert_info_t previous_player) const override {
    state->current_player_ = previous_player;
  }

 private:
  player_id_t player_id_;
};

class AssignmentAction : public ModifyingApplication {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kModifier;
  using revert_info_t = variable_value_t;

  explicit AssignmentAction(variable_id_t variable_id,
                            std::unique_ptr<ArithmeticOperation> value,
                            uint index)
      : ModifyingApplication(index),
        variable_id_(variable_id),
        value_(std::move(value)) {}

  revert_info_t Apply(GameState *state) const override {
    variable_value_t previous_value = state->variables_values_[variable_id_];
    state->variables_values_[variable_id_] = value_->Value(*state);
    return previous_value;
  }

  void Revert(GameState *state, revert_info_t previous_value) const override {
    state->variables_values_[variable_id_] = previous_value;
  }

 private:
  variable_id_t variable_id_;
  std::unique_ptr<ArithmeticOperation> value_;
};

class VariableBoundsTest {
 public:
  static constexpr ActionTypeTrait kind = ActionTypeTrait::kCheck;

  explicit VariableBoundsTest(variable_id_t variable_id)
      : variable_id_(variable_id) {}

  inline bool Check(GameState *state) const {
    return state->variables_values()[variable_id_] <=
           state->declarations().variable_bound(variable_id_);
  }

 private:
  variable_id_t variable_id_;
};
}  // namespace rbg

#endif  // RBGGAMEMANAGER_ACTIONS_H
