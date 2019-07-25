//
// Created by shrum on 12/07/19.
//

#ifndef RBGGAMEMANAGER_ACTIONS_H
#define RBGGAMEMANAGER_ACTIONS_H

#include "arithmetic_operation.h"
#include <vector>
#include <game_state/declarations.h>
#include "stl_extension/resetabble_bitarray_stack.h"
#include "modifying_application.h"
#include <game_state/game_state.h>

namespace rbg {
  class GameState;

  class SearchStepsCollection;

  class SearchStepsPoint;
}

namespace rbg {


  class VisitedCheckTest {
  public:
    static constexpr ActionTypeTrait type = ActionTypeTrait::CHECK;

    explicit VisitedCheckTest(ResetableBitArrayStackChunk bit_array_stack_chunk)
        : stack_chunk_(bit_array_stack_chunk) {
    }

    always_inline   bool Check(GameState *state) {
      bool result = stack_chunk_[state->current_pos_];
      stack_chunk_.set(state->current_pos_);
      return !result;
    }

  private:
    ResetableBitArrayStackChunk stack_chunk_;
  };

  class ShiftAction {
  public:
    static constexpr ActionTypeTrait type = ActionTypeTrait::APPLICATION;
    using revert_info_t = vertex_id_t;

    explicit ShiftAction(shift_edge_id edge_id) : edge_id_(edge_id) {}

    always_inline     revert_info_t Apply(GameState *state) const {

      vertex_id_t previous_vertex = state->current_pos_;
      state->current_pos_ = state->declarations().board_description.NextVertex(state->current_pos_, edge_id_);
      return previous_vertex;

    }

    always_inline     void Revert(GameState *state, revert_info_t previous_vertex) const {

      state->current_pos_ = previous_vertex;
    }

  private:
    shift_edge_id edge_id_;
  };


  class OutOfBoundsTest {
  public:
    static constexpr ActionTypeTrait type = ActionTypeTrait::CHECK;

    always_inline     bool Check(GameState *state) const {

      return state->current_pos() < state->declarations().board_description.vertices_count();
    }
  };


  class OnTest {
  public:
    static constexpr ActionTypeTrait type = ActionTypeTrait::CHECK;

    explicit OnTest(std::vector<bool> pieces) : pieces_(std::move(pieces)) {}

    always_inline     bool Check(GameState *state) const {

      return pieces_[state->board_.at(state->current_pos_)];
    }

  private:
    std::vector<bool> pieces_;
  };


  class PlayerTest {
  public:
    static constexpr ActionTypeTrait type = ActionTypeTrait::CHECK;

    explicit PlayerTest(player_id_t player) : player_(player) {}

    always_inline     bool Check(GameState *state) const {

      return state->current_player_ == player_;
    }

  private:
    player_id_t player_;
  };


  class LessEqualComparisonTest {
  public:

    static constexpr ActionTypeTrait type = ActionTypeTrait::CHECK;

    explicit LessEqualComparisonTest(std::unique_ptr<ArithmeticOperation> left,
                                     std::unique_ptr<ArithmeticOperation> right) : left_(std::move(left)),
                                                                                   right_(std::move(right)) {}

    always_inline     bool Check(GameState *state) const {

      return left_->Value(*state) <= right_->Value(*state);
    }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };


  class LessComparisonTest {
  public:

    static constexpr ActionTypeTrait type = ActionTypeTrait::CHECK;

    explicit LessComparisonTest(std::unique_ptr<ArithmeticOperation> left,
                                std::unique_ptr<ArithmeticOperation> right) : left_(std::move(left)),
                                                                              right_(std::move(right)) {}

    always_inline     bool Check(GameState *state) const {

      return left_->Value(*state) < right_->Value(*state);
    }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };


  class EqualComparisonTest {
  public:

    static constexpr ActionTypeTrait type = ActionTypeTrait::CHECK;

    explicit EqualComparisonTest(std::unique_ptr<ArithmeticOperation> left,
                                 std::unique_ptr<ArithmeticOperation> right) : left_(std::move(left)),
                                                                               right_(std::move(right)) {}

    always_inline     bool Check(GameState *state) const {

      return left_->Value(*state) == right_->Value(*state);
    }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };


  class NotEqualComparisonTest {
  public:

    static constexpr ActionTypeTrait type = ActionTypeTrait::CHECK;

    explicit NotEqualComparisonTest(std::unique_ptr<ArithmeticOperation> left,
                                    std::unique_ptr<ArithmeticOperation> right) : left_(std::move(left)),
                                                                                  right_(std::move(right)) {}

    always_inline     bool Check(GameState *state) const {

      return left_->Value(*state) != right_->Value(*state);
    }


  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };


  class ConditionCheckTest {
  public:

    static constexpr ActionTypeTrait type = ActionTypeTrait::CHECK;

    explicit ConditionCheckTest(std::unique_ptr<SearchStepsPoint> steps_point)
        : steps_point_(std::move(steps_point)) {};

    always_inline     bool Check(GameState *state) const {

      return steps_point_->current()->RunAndFindEnd(state);
    }


  private:
    std::unique_ptr<SearchStepsPoint> steps_point_;
  };


  class NegatedConditionCheckTest {
  public:

    static constexpr ActionTypeTrait type = ActionTypeTrait::CHECK;

    explicit NegatedConditionCheckTest(std::unique_ptr<SearchStepsPoint> steps_point)
        : steps_point_(std::move(steps_point)) {};

    always_inline     bool Check(GameState *state) const {

      return !steps_point_->current()->RunAndFindEnd(state);
    }


  private:
    std::unique_ptr<SearchStepsPoint> steps_point_;
  };


  class OffApplication : public ModifyingApplication {
  public:
    static constexpr ActionTypeTrait type = ActionTypeTrait::MODIFIER;
    using revert_info_t = piece_id_t;

    explicit OffApplication(revert_info_t piece_id, uint index) : ModifyingApplication(index), piece_id_(piece_id) {}

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
    static constexpr ActionTypeTrait type = ActionTypeTrait::SWITCH;
    using revert_info_t = player_id_t;

    explicit PlayerSwitchApplication(player_id_t player_id, uint index) : ModifyingApplication(index),
                                                                          player_id_(player_id) {}

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
    static constexpr ActionTypeTrait type = ActionTypeTrait::MODIFIER;
    using revert_info_t = variable_value_t;

    explicit AssignmentAction(variable_id_t variable_id, std::unique_ptr<ArithmeticOperation> value, uint index)
        : ModifyingApplication(index), variable_id_(variable_id), value_(std::move(value)) {}

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
    static constexpr ActionTypeTrait type = ActionTypeTrait::CHECK;

    explicit VariableBoundsTest(variable_id_t variable_id) : variable_id_(variable_id) {}

    always_inline     bool Check(GameState *state) const {

      return state->variables_values()[variable_id_] <= state->declarations().variables_bounds[variable_id_];
    }

  private:
    variable_id_t variable_id_;
  };

  template<typename NormalApplication>
  class WeirdMultipleApplication : public NormalApplication {
    static constexpr ActionTypeTrait type = ActionTypeTrait::MULTIPLE_APPLICATION;

    explicit WeirdMultipleApplication(NormalApplication normal_application) : normal_application_(normal_application) {}

    template<typename Function>
    void Run(GameState *state, Function next) {
      normal_application_.Apply(state);
      next();
      normal_application_.Revert(state);
    }

  private:
    NormalApplication normal_application_;

  };
}


#endif //RBGGAMEMANAGER_ACTIONS_H
