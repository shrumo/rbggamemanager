#include <utility>

//
// Created by shrum on 10.06.19.
//

#ifndef RBGGAMEMANAGER_SEARCHSTEP_H
#define RBGGAMEMANAGER_SEARCHSTEP_H

#include "../gamestate.h"
#include <vector>
#include "resetabblebitarraystack.h"

namespace rbg {
  struct ModifierApplication {
    vertex_id_t vertex;
    uint modifier_index;
  };

  class SearchStep {
  public:
    virtual void
    Run(GameState &, std::vector<ModifierApplication> &, std::vector<std::vector<ModifierApplication>> &) = 0;
  };

  class EmptySearchStep : public SearchStep {
  public:
    void Run(GameState &, std::vector<ModifierApplication> &, std::vector<std::vector<ModifierApplication>> &) final {}
  };

  class SingleSearchStep : public SearchStep {
  protected:
    void SetNextSearchStep(SearchStep *next) {
      next_ = next;
    }

    void RunNextStep(GameState &state,
                     std::vector<ModifierApplication> &applied_modifiers,
                     std::vector<std::vector<ModifierApplication>> &moves) {
      next_->Run(state, applied_modifiers, moves);
    }

  private:
    SearchStep *next_ = {};
  };


  class VisitedCheckSearchStep : public SingleSearchStep {
  public:
    explicit VisitedCheckSearchStep(ResetableBitArrayStackChunk bit_array_stack_chunk)
        : stack_chunk_(bit_array_stack_chunk) {
    }

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (stack_chunk_[state.current_pos_])
        return;
      RunNextStep(state, applied_modifiers, moves);
    }

  private:
    ResetableBitArrayStackChunk stack_chunk_;
  };

  class ModifyingSearchStep : public SingleSearchStep {
  public:
    explicit ModifyingSearchStep(uint index) : index_(index) {}

    virtual void Apply(GameState &, vertex_id_t vertex) = 0;

    uint index() const {
      return index_;
    }

  private:
    uint index_;
  };

  class ArithmeticValue {
  public:
    virtual variable_value_t Value(const GameState &s) const = 0;
  };

  class VariableValue : public ArithmeticValue {
  public:
    explicit VariableValue(variable_id_t variable_id) : variable_id_(variable_id) {}

    variable_value_t Value(const GameState &s) const override {
      return s.variables_values_[variable_id_];
    }

  private:
    variable_id_t variable_id_;
  };

  class ConstantValue : public ArithmeticValue {
  public:
    explicit ConstantValue(variable_value_t value) : value_(value) {}

    variable_value_t Value(const GameState &) const override {
      return value_;
    }

  private:
    variable_value_t value_;
  };

  class SumValue : public ArithmeticValue {
  public:
    explicit SumValue(std::vector<const ArithmeticValue *> summands) : summands_(std::move(summands)) {}

    variable_value_t Value(const GameState &s) const override {
      variable_value_t result = summands_[0]->Value(s);
      for (size_t i = 1; i < summands_.size(); i++) {
        result += summands_[i]->Value(s);
      }
      return result;
    }

  private:
    std::vector<const ArithmeticValue *> summands_;
  };

  class SubtractionValue : public ArithmeticValue {
  public:
    explicit SubtractionValue(std::vector<const ArithmeticValue *> elements) : elements_(std::move(elements)) {}

    variable_value_t Value(const GameState &s) const override {
      variable_value_t result = elements_[0]->Value(s);
      for (size_t i = 1; i < elements_.size(); i++) {
        result -= elements_[i]->Value(s);
      }
      return result;
    }

  private:
    std::vector<const ArithmeticValue *> elements_;
  };

  class ProductValue : public ArithmeticValue {
  public:
    explicit ProductValue(std::vector<const ArithmeticValue *> factors) : factors_(std::move(factors)) {}

    variable_value_t Value(const GameState &s) const override {
      variable_value_t result = factors_[0]->Value(s);
      for (size_t i = 1; i < factors_.size(); i++) {
        result *= factors_[i]->Value(s);
      }
      return result;
    }

  private:
    std::vector<const ArithmeticValue *> factors_;
  };

  class DivisionValue : public ArithmeticValue {
  public:
    explicit DivisionValue(std::vector<const ArithmeticValue *> elements) : elements_(std::move(elements)) {}

    variable_value_t Value(const GameState &s) const override {
      variable_value_t result = elements_[0]->Value(s);
      for (size_t i = 1; i < elements_.size(); i++) {
        result /= elements_[i]->Value(s);
      }
      return result;
    }

  private:
    std::vector<const ArithmeticValue *> elements_;
  };

  class ShiftStep : public SingleSearchStep {
  public:
    explicit ShiftStep(edge_id_t edge_id) : edge_id_(edge_id) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      vertex_id_t old_pos = state.current_pos_;
      state.current_pos_ = state.board_.NextVertex(state.current_pos_, edge_id_);
      RunNextStep(state, applied_modifiers, moves);
      state.current_pos_ = old_pos;
    }

  private:
    edge_id_t edge_id_;
  };

  class OnStep : public SingleSearchStep {
  public:
    explicit OnStep(std::vector<bool> pieces) : pieces_(std::move(pieces)) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (!pieces_[state.board_[state.current_pos_]])
        return;
      RunNextStep(state, applied_modifiers, moves);
    }

  private:
    std::vector<bool> pieces_;
  };

  class ArithemticLessEqualComparison : public SingleSearchStep {
  public:
    explicit ArithemticLessEqualComparison(const ArithmeticValue *left, const ArithmeticValue *right) : left_(left),
                                                                                                        right_(right) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (left_->Value(state) <= right_->Value(state))
        return;
      RunNextStep(state, applied_modifiers, moves);
    }

  private:
    const ArithmeticValue *left_, *right_;
  };


  class ArithemticLessComparison : public SingleSearchStep {
  public:
    explicit ArithemticLessComparison(const ArithmeticValue *left, const ArithmeticValue *right) : left_(left),
                                                                                                   right_(right) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (left_->Value(state) < right_->Value(state))
        return;
      RunNextStep(state, applied_modifiers, moves);
    }

  private:
    const ArithmeticValue *left_, *right_;
  };


  class ArithemticEqualComparison : public SingleSearchStep {
  public:
    explicit ArithemticEqualComparison(const ArithmeticValue *left, const ArithmeticValue *right) : left_(left),
                                                                                                    right_(right) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (left_->Value(state) == right_->Value(state))
        return;
      RunNextStep(state, applied_modifiers, moves);
    }

  private:
    const ArithmeticValue *left_, *right_;
  };

  class ConditionCheckStep : public SingleSearchStep {
  public:
    explicit ConditionCheckStep(SearchStep *search) : search_(search) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      std::vector<std::vector<ModifierApplication >> condition_moves;
      std::vector<ModifierApplication> condition_modifiers;
      search_->Run(state, condition_modifiers, condition_moves);
      if (moves.empty())
        return;
      RunNextStep(state, applied_modifiers, moves);
    }

  private:
    SearchStep *search_;
  };

  class NegatedConditionCheckStep : public SingleSearchStep {
  public:
    explicit NegatedConditionCheckStep(SearchStep *search) : search_(search) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      std::vector<std::vector<ModifierApplication >> condition_moves;
      std::vector<ModifierApplication> condition_modifiers;
      search_->Run(state, condition_modifiers, condition_moves);
      if (!moves.empty())
        return;
      RunNextStep(state, applied_modifiers, moves);
    }

  private:
    SearchStep *search_;
  };

  class OffStep : public ModifyingSearchStep {
  public:
    explicit OffStep(piece_id_t piece_id, uint index) : ModifyingSearchStep(index), piece_id_(piece_id) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      piece_id_t previous = state.board_[state.current_pos_];
      state.board_[state.current_pos_] = piece_id_;
      applied_modifiers.push_back({state.current_pos_, index()});
      RunNextStep(state, applied_modifiers, moves);
      applied_modifiers.pop_back();
      state.board_[state.current_pos_] = previous;
    }

  private:
    piece_id_t piece_id_;
  };

  class PlayerSwitchStep : public ModifyingSearchStep {
  public:
    explicit PlayerSwitchStep(player_id_t player_id, uint index) : ModifyingSearchStep(index), player_id_(player_id) {}

    void Run(GameState &state, std::vector<ModifierApplication> &moves,
             std::vector<std::vector<ModifierApplication>> &result) override {
      player_id_t previous = state.current_player_;
      state.current_player_ = player_id_;
      moves.push_back({state.current_pos_, index()});
      result.push_back(moves);
      moves.pop_back();
      state.current_player_ = previous;
    }

  private:
    player_id_t player_id_;
  };

  class AssignmentStep : public ModifyingSearchStep {
  public:
    explicit AssignmentStep(variable_id_t variable_id, const ArithmeticValue *value, uint index)
        : ModifyingSearchStep(index), variable_id_(variable_id), value_(value) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      variable_value_t previous = state.variables_values_[variable_id_];
      state.variables_values_[variable_id_] = value_->Value(state);
      applied_modifiers.push_back({state.current_pos_, index()});
      RunNextStep(state, applied_modifiers, moves);
      applied_modifiers.pop_back();
      state.variables_values_[variable_id_] = previous;
    }

  private:
    variable_id_t variable_id_;
    const ArithmeticValue *value_;
  };

}


#endif //RBGGAMEMANAGER_SEARCHSTEP_H
