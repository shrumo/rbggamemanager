#include <utility>

//
// Created by shrum on 10.06.19.
//

#ifndef RBGGAMEMANAGER_SEARCHSTEP_H
#define RBGGAMEMANAGER_SEARCHSTEP_H


#include "arithmetic_operation.h"
#include <vector>
#include <gamestate/gamestate.h>
#include <gamestate/declarations.h>
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

    virtual void AddNextSearchStep(SearchStep *next) = 0;

  };

  class EndStep : public SearchStep {
  public:
    void
    Run(GameState &, std::vector<ModifierApplication> &, std::vector<std::vector<ModifierApplication>> &) final {}

    void AddNextSearchStep(SearchStep *) {
      assert(false && "Next step cannot be added to the ending step.");
    }
  };

  class SingleSearchStep : public SearchStep {
  public:
    void AddNextSearchStep(SearchStep *next) override {
      assert(next_ == nullptr && "Single search step can have only one next step.");
      next_ = next;
    }

  protected:
    void RunNextStep(GameState &state,
                     std::vector<ModifierApplication> &applied_modifiers,
                     std::vector<std::vector<ModifierApplication>> &moves) {
      next_->Run(state, applied_modifiers, moves);
    }

    SearchStep *next_ = {};
  };

  class MultipleSearchStep : public SearchStep {
  public:
    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) final {
      for (auto step : next_steps_) {
        step->Run(state, applied_modifiers, moves);
      }
    }

    void AddNextSearchStep(SearchStep *next) override {
      next_steps_.push_back(next);
    }

  private:
    std::vector<SearchStep *> next_steps_;
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

  class ShiftStep : public SingleSearchStep {
  public:
    explicit ShiftStep(shift_edge_id_t edge_id) : edge_id_(edge_id) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      vertex_id_t old_pos = state.current_pos_;
      state.current_pos_ = state.board_.NextVertex(state.current_pos_, edge_id_);
      RunNextStep(state, applied_modifiers, moves);
      state.current_pos_ = old_pos;
    }

  private:
    shift_edge_id_t edge_id_;
  };

  class OnStep : public SingleSearchStep {
  public:
    explicit OnStep(std::vector<bool> pieces) : pieces_(std::move(pieces)) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (!pieces_[state.board_.at(state.current_pos_)])
        return;
      RunNextStep(state, applied_modifiers, moves);
    }

  private:
    std::vector<bool> pieces_;
  };


  class ArithmeticLessEqualComparison : public SingleSearchStep {
  public:
    explicit ArithmeticLessEqualComparison(std::unique_ptr<ArithmeticOperation> left,
                                           std::unique_ptr<ArithmeticOperation> right) : left_(std::move(left)),
                                                                                         right_(std::move(right)) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (left_->Value(state) <= right_->Value(state))
        return;
      RunNextStep(state, applied_modifiers, moves);
    }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };


  class ArithmeticLessComparison : public SingleSearchStep {
  public:
    explicit ArithmeticLessComparison(std::unique_ptr<ArithmeticOperation> left,
                                      std::unique_ptr<ArithmeticOperation> right) : left_(std::move(left)),
                                                                                    right_(std::move(right)) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (left_->Value(state) < right_->Value(state))
        return;
      RunNextStep(state, applied_modifiers, moves);
    }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };


  class ArithmeticEqualComparison : public SingleSearchStep {
  public:
    explicit ArithmeticEqualComparison(std::unique_ptr<ArithmeticOperation> left,
                                       std::unique_ptr<ArithmeticOperation> right) : left_(std::move(left)),
                                                                                     right_(std::move(right)) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (left_->Value(state) == right_->Value(state))
        return;
      RunNextStep(state, applied_modifiers, moves);
    }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };


  class ArithmeticNotEqualComparison : public SingleSearchStep {
  public:
    explicit ArithmeticNotEqualComparison(std::unique_ptr<ArithmeticOperation> left,
                                          std::unique_ptr<ArithmeticOperation> right) : left_(std::move(left)),
                                                                                        right_(std::move(right)) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (left_->Value(state) != right_->Value(state))
        return;
      RunNextStep(state, applied_modifiers, moves);
    }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
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
      piece_id_t previous = state.board_.at(state.current_pos_);
      state.board_.set(state.current_pos_, piece_id_);
      applied_modifiers.push_back({state.current_pos_, index()});
      RunNextStep(state, applied_modifiers, moves);
      applied_modifiers.pop_back();
      state.board_.set(state.current_pos_, previous);
    }

    void Apply(GameState &state, vertex_id_t vertex) override {
      assert(false && "Not yet implemented.");
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


    void Apply(GameState &state, vertex_id_t vertex) override {
      assert(false && "Not yet implemented.");
    }


  private:
    player_id_t player_id_;
  };

  class AssignmentStep : public ModifyingSearchStep {
  public:
    explicit AssignmentStep(variable_id_t variable_id, std::unique_ptr<ArithmeticOperation> value, uint index)
        : ModifyingSearchStep(index), variable_id_(variable_id), value_(std::move(value)) {}

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      variable_value_t previous = state.variables_values_[variable_id_];
      state.variables_values_[variable_id_] = value_->Value(state);
      applied_modifiers.push_back({state.current_pos_, index()});
      RunNextStep(state, applied_modifiers, moves);
      applied_modifiers.pop_back();
      state.variables_values_[variable_id_] = previous;
    }


    void Apply(GameState &state, vertex_id_t vertex) override {
      assert(false && "Not yet implemented.");
    }


  private:
    variable_id_t variable_id_;
    std::unique_ptr<ArithmeticOperation> value_;
  };

}


#endif //RBGGAMEMANAGER_SEARCHSTEP_H
