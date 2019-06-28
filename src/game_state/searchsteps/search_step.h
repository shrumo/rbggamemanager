#include <utility>

//
// Created by shrum on 10.06.19.
//

#ifndef RBGGAMEMANAGER_SEARCHSTEP_H
#define RBGGAMEMANAGER_SEARCHSTEP_H


#include "arithmetic_operation.h"
#include <vector>
#include <game_state/declarations.h>
#include "resetabblebitarraystack.h"

namespace rbg {
  class GameState;
}

namespace rbg {
  struct ModifierApplication {
    vertex_id_t vertex;
    uint modifier_index;
  };

  class SearchStep {
  public:
    virtual void
    Run(GameState &, std::vector<ModifierApplication> &, std::vector<std::vector<ModifierApplication>> &) = 0;

    virtual bool
    ApplyFirstFound(GameState &) = 0;

    virtual bool
    RunUntilFound(GameState &, std::vector<ModifierApplication> &) = 0;

    virtual void AddNextSearchStep(SearchStep *next) = 0;

    virtual ~SearchStep() = default;

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

    bool ApplyNextStepUntilFound(GameState &state) {
      return next_->ApplyFirstFound(state);
    }


    bool RunNextStepUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) {
      return next_->RunUntilFound(state, applied_modifiers);
    }

    SearchStep *next_ = {};
  };


  class ModifyingSearchStep : public SingleSearchStep {
  public:
    explicit ModifyingSearchStep(uint index) : index_(index) {}

    virtual void Apply(GameState &state, vertex_id_t) const = 0;

    uint index() const {
      return index_;
    }

  private:
    uint index_;
  };

  class MultipleSearchStep : public SearchStep {
  public:
    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) final {
      for (auto step : next_steps_) {
        step->Run(state, applied_modifiers, moves);
      }
    }

    bool ApplyFirstFound(GameState &state) final {
      for (auto step : next_steps_) {
        if (step->ApplyFirstFound(state))
          return true;
      }
      return false;
    }

    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) final {
      for (auto step : next_steps_) {
        if (step->RunUntilFound(state, applied_modifiers))
          return true;
      }
      return false;
    }


    void AddNextSearchStep(SearchStep *next) override {
      next_steps_.push_back(next);
    }

  private:
    std::vector<SearchStep *> next_steps_;
  };

  class EndStep : public SearchStep {
  public:
    void
    Run(GameState &, std::vector<ModifierApplication> &, std::vector<std::vector<ModifierApplication>> &) final {}

    bool ApplyFirstFound(GameState &) final { return false; }

    bool RunUntilFound(GameState &, std::vector<ModifierApplication> &) final { return false; }

    void AddNextSearchStep(SearchStep *) final {
      assert(false && "Next step cannot be added to the ending step.");
    }
  };

  class VisitedCheckSearchStep : public SingleSearchStep {
  public:
    explicit VisitedCheckSearchStep(ResetableBitArrayStackChunk bit_array_stack_chunk)
        : stack_chunk_(bit_array_stack_chunk) {
    }

    bool Apply(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Apply(state))
        RunNextStep(state, applied_modifiers, moves);
    }

    bool
    ApplyFirstFound(GameState &state) override {
      if (Apply(state))
        return ApplyNextStepUntilFound(state);
      return false;
    }

    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) final {
      if (Apply(state))
        return RunNextStepUntilFound(state, applied_modifiers);
      return false;
    }

  private:
    ResetableBitArrayStackChunk stack_chunk_;
  };


  class ShiftStep : public SingleSearchStep {
  public:
    explicit ShiftStep(shift_edge_id_t edge_id) : edge_id_(edge_id) {}

    using revert_info_t = vertex_id_t;

    bool ApplyReversible(GameState &state, revert_info_t &revert_info);

    void Revert(GameState &state, const revert_info_t &revert_info);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      revert_info_t revert_info;
      if (ApplyReversible(state, revert_info)) {
        RunNextStep(state, applied_modifiers, moves);
      }
      Revert(state, revert_info);
    }

    bool
    ApplyFirstFound(GameState &state) override {
      revert_info_t revert_info;
      if (ApplyReversible(state, revert_info)) {
        if (ApplyNextStepUntilFound(state))
          return true;
      }
      Revert(state, revert_info);
      return false;
    }


    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) override {
      revert_info_t revert_info;
      bool result = false;
      if (ApplyReversible(state, revert_info)) {
        result = RunNextStepUntilFound(state, applied_modifiers);
      }
      Revert(state, revert_info);
      return result;
    }

  private:
    shift_edge_id_t edge_id_;
  };

  class OnStep : public SingleSearchStep {
  public:
    explicit OnStep(std::vector<bool> pieces) : pieces_(std::move(pieces)) {}

    bool Apply(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Apply(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state) override {
      if (Apply(state)) {
        if (ApplyNextStepUntilFound(state))
          return true;
      }
      return false;
    }

    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) override {
      bool result = false;
      if (Apply(state)) {
        result = RunNextStepUntilFound(state, applied_modifiers);
      }
      return result;
    }

  private:
    std::vector<bool> pieces_;
  };

  class PlayerCheckStep : public SingleSearchStep {
  public:
    explicit PlayerCheckStep(player_id_t player) : player_(player) {}

    bool Apply(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Apply(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state) override {
      if (Apply(state)) {
        if (ApplyNextStepUntilFound(state))
          return true;
      }
      return false;
    }

    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) override {
      bool result = false;
      if (Apply(state)) {
        result = RunNextStepUntilFound(state, applied_modifiers);
      }
      return result;
    }

  private:
    player_id_t player_;
  };


  class ArithmeticLessEqualComparison : public SingleSearchStep {
  public:
    explicit ArithmeticLessEqualComparison(std::unique_ptr<ArithmeticOperation> left,
                                           std::unique_ptr<ArithmeticOperation> right) : left_(std::move(left)),
                                                                                         right_(std::move(right)) {}

    bool Apply(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Apply(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state) override {
      if (Apply(state)) {
        if (ApplyNextStepUntilFound(state))
          return true;
      }
      return false;
    }


    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) override {
      bool result = false;
      if (Apply(state)) {
        result = RunNextStepUntilFound(state, applied_modifiers);
      }
      return result;
    }


  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };


  class ArithmeticLessComparison : public SingleSearchStep {
  public:
    explicit ArithmeticLessComparison(std::unique_ptr<ArithmeticOperation> left,
                                      std::unique_ptr<ArithmeticOperation> right) : left_(std::move(left)),
                                                                                    right_(std::move(right)) {}

    bool Apply(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Apply(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state) override {
      if (Apply(state)) {
        if (ApplyNextStepUntilFound(state))
          return true;
      }
      return false;
    }


    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) override {
      bool result = false;
      if (Apply(state)) {
        result = RunNextStepUntilFound(state, applied_modifiers);
      }
      return result;
    }


  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };


  class ArithmeticEqualComparison : public SingleSearchStep {
  public:
    explicit ArithmeticEqualComparison(std::unique_ptr<ArithmeticOperation> left,
                                       std::unique_ptr<ArithmeticOperation> right) : left_(std::move(left)),
                                                                                     right_(std::move(right)) {}

    bool Apply(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Apply(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state) override {
      if (Apply(state)) {
        if (ApplyNextStepUntilFound(state))
          return true;
      }
      return false;
    }


    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) override {
      bool result = false;
      if (Apply(state)) {
        result = RunNextStepUntilFound(state, applied_modifiers);
      }
      return result;
    }


  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };


  class ArithmeticNotEqualComparison : public SingleSearchStep {
  public:
    explicit ArithmeticNotEqualComparison(std::unique_ptr<ArithmeticOperation> left,
                                          std::unique_ptr<ArithmeticOperation> right) : left_(std::move(left)),
                                                                                        right_(std::move(right)) {}

    bool Apply(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Apply(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state) override {
      if (Apply(state)) {
        if (ApplyNextStepUntilFound(state))
          return true;
      }
      return false;
    }


    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) override {
      bool result = false;
      if (Apply(state)) {
        result = RunNextStepUntilFound(state, applied_modifiers);
      }
      return result;
    }


  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };

  class ConditionCheckStep : public SingleSearchStep {
  public:
    explicit ConditionCheckStep(SearchStep *search) : search_(search) {}

    bool Apply(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Apply(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state) override {
      if (Apply(state)) {
        if (ApplyNextStepUntilFound(state))
          return true;
      }
      return false;
    }


    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) override {
      bool result = false;
      if (Apply(state)) {
        result = RunNextStepUntilFound(state, applied_modifiers);
      }
      return result;
    }

  private:
    SearchStep *search_;
  };

  class NegatedConditionCheckStep : public SingleSearchStep {
  public:
    explicit NegatedConditionCheckStep(SearchStep *search) : search_(search) {}

    bool Apply(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Apply(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state) override {
      if (Apply(state)) {
        if (ApplyNextStepUntilFound(state))
          return true;
      }
      return false;
    }


    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) override {
      bool result = false;
      if (Apply(state)) {
        result = RunNextStepUntilFound(state, applied_modifiers);
      }
      return result;
    }

  private:
    SearchStep *search_;
  };

  class OffStep : public ModifyingSearchStep {
  public:
    explicit OffStep(piece_id_t piece_id, uint index) : ModifyingSearchStep(index), piece_id_(piece_id) {}

    using revert_info_t = piece_id_t;

    void ApplyReversible(GameState &state, revert_info_t &revert_info) const;

    void Revert(GameState &state, const revert_info_t &revert_info);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override;

    bool
    ApplyFirstFound(GameState &state) override;


    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) override;

    void Apply(GameState &state, vertex_id_t vertex) const override;

  private:
    piece_id_t piece_id_;
  };

  class PlayerSwitchStep : public ModifyingSearchStep {
  public:
    explicit PlayerSwitchStep(player_id_t player_id, uint index) : ModifyingSearchStep(index), player_id_(player_id) {}

    using revert_info_t = player_id_t;

    void ApplyReversible(GameState &state, revert_info_t &revert_info) const;

    void Revert(GameState &state, const revert_info_t &revert_info);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override;

    bool
    ApplyFirstFound(GameState &state) override;


    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) override;

    void Apply(GameState &state, vertex_id_t vertex) const override;

  private:
    player_id_t player_id_;
  };

  class AssignmentStep : public ModifyingSearchStep {
  public:
    explicit AssignmentStep(variable_id_t variable_id, std::unique_ptr<ArithmeticOperation> value, uint index)
        : ModifyingSearchStep(index), variable_id_(variable_id), value_(std::move(value)) {}

    using revert_info_t = variable_value_t;

    void ApplyReversible(GameState &state, revert_info_t &revert_info) const;

    void Revert(GameState &state, const revert_info_t &revert_info);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override;

    bool
    ApplyFirstFound(GameState &state) override;


    bool
    RunUntilFound(GameState &state, std::vector<ModifierApplication> &applied_modifiers) override;

    void Apply(GameState &state, vertex_id_t vertex) const override;


  private:
    variable_id_t variable_id_;
    std::unique_ptr<ArithmeticOperation> value_;
  };
}


#endif //RBGGAMEMANAGER_SEARCHSTEP_H
