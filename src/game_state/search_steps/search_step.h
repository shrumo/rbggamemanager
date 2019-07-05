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
  class SearchStepsCollection;

  class SearchStepsPoint;
}

namespace rbg {
  struct ModifierApplication {
    vertex_id_t vertex;
    uint modifier_index;

    bool operator==(const ModifierApplication &b) const {
      return vertex == b.vertex && modifier_index == b.modifier_index;
    }
  };

  struct StepRevertInformation;

  class SearchStep {
  public:
    virtual void
    Run(GameState &, std::vector<ModifierApplication> &, std::vector<std::vector<ModifierApplication>> &) = 0;

    virtual bool
    ApplyFirstFound(GameState &, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) = 0;

    virtual bool
    EndStepReachable(GameState &) = 0;

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

    bool ApplyNextStepUntilFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                                 vertex_id_t revert_vertex) {
      return next_->ApplyFirstFound(state, reverting_information, revert_vertex);
    }


    bool EndStepReachableFromNext(GameState &state) {
      return next_->EndStepReachable(state);
    }

    SearchStep *next_ = {};
  };


  class ModifyingSearchStep : public SingleSearchStep {
  public:
    explicit ModifyingSearchStep(uint index) : index_(index) {}

    virtual StepRevertInformation ApplyReversibleAtVertex(GameState &state, vertex_id_t) const = 0;

    virtual void RevertFromRevertInfo(GameState &state, const StepRevertInformation &) const = 0;

    uint index() const {
      return index_;
    }

    SearchStep *next_step() const {
      return next_;
    }

  private:
    uint index_;
  };


  struct StepRevertInformation {
    const ModifyingSearchStep &modifying_step;
    vertex_id_t vertex;
    name_id_t revert_info;
  };


  class MultipleSearchStep : public SearchStep {
  public:
    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) final {
      for (auto step : next_steps_) {
        step->Run(state, applied_modifiers, moves);
      }
    }

    bool ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                         vertex_id_t revert_vertex) final {
      for (auto step : next_steps_) {
        if (step->ApplyFirstFound(state, reverting_information, revert_vertex))
          return true;
      }
      return false;
    }

    bool
    EndStepReachable(GameState &state) final {
      for (auto step : next_steps_) {
        if (step->EndStepReachable(state))
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

    bool ApplyFirstFound(GameState &, std::vector<StepRevertInformation> &, vertex_id_t) final { return false; }

    bool EndStepReachable(GameState &) final { return true; }

    void AddNextSearchStep(SearchStep *) final {
      assert(false && "Next step cannot be added to the ending step.");
    }
  };

  class VisitedCheckSearchStep : public SingleSearchStep {
  public:
    explicit VisitedCheckSearchStep(ResetableBitArrayStackChunk bit_array_stack_chunk)
        : stack_chunk_(bit_array_stack_chunk) {
    }

    bool Check(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Check(state))
        RunNextStep(state, applied_modifiers, moves);
    }

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override {
      if (Check(state))
        return ApplyNextStepUntilFound(state, reverting_information, revert_vertex);
      return false;
    }

    bool
    EndStepReachable(GameState &state) final {
      if (Check(state))
        return EndStepReachableFromNext(state);
      return false;
    }

  private:
    ResetableBitArrayStackChunk stack_chunk_;
  };


  class ShiftStep : public SingleSearchStep {
  public:
    explicit ShiftStep(shift_edge_id_t edge_id) : edge_id_(edge_id) {}

    bool SetAndCheck(GameState &state, vertex_id_t &previous_vertex);

    void Revert(GameState &state, vertex_id_t previous_vertex);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      vertex_id_t previous_vertex;
      if (SetAndCheck(state, previous_vertex)) {
        RunNextStep(state, applied_modifiers, moves);
      }
      Revert(state, previous_vertex);
    }

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override {
      vertex_id_t revert_info;
      if (SetAndCheck(state, revert_info)) {
        if (ApplyNextStepUntilFound(state, reverting_information, revert_vertex))
          return true;
      }
      Revert(state, revert_info);
      return false;
    }


    bool
    EndStepReachable(GameState &state) override {
      vertex_id_t previous_vertex;
      bool result = false;
      if (SetAndCheck(state, previous_vertex)) {
        result = EndStepReachableFromNext(state);
      }
      Revert(state, previous_vertex);
      return result;
    }

  private:
    shift_edge_id_t edge_id_;
  };

  class OnStep : public SingleSearchStep {
  public:
    explicit OnStep(std::vector<bool> pieces) : pieces_(std::move(pieces)) {}

    bool Check(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Check(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override {
      if (Check(state)) {
        if (ApplyNextStepUntilFound(state, reverting_information, revert_vertex))
          return true;
      }
      return false;
    }

    bool
    EndStepReachable(GameState &state) override {
      bool result = false;
      if (Check(state)) {
        result = EndStepReachableFromNext(state);
      }
      return result;
    }

  private:
    std::vector<bool> pieces_;
  };

  class PlayerCheckStep : public SingleSearchStep {
  public:
    explicit PlayerCheckStep(player_id_t player) : player_(player) {}

    bool Check(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Check(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override {
      if (Check(state)) {
        if (ApplyNextStepUntilFound(state, reverting_information, revert_vertex))
          return true;
      }
      return false;
    }

    bool
    EndStepReachable(GameState &state) override {
      bool result = false;
      if (Check(state)) {
        result = EndStepReachableFromNext(state);
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

    bool Check(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Check(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override {
      if (Check(state)) {
        if (ApplyNextStepUntilFound(state, reverting_information, revert_vertex))
          return true;
      }
      return false;
    }


    bool
    EndStepReachable(GameState &state) override {
      bool result = false;
      if (Check(state)) {
        result = EndStepReachableFromNext(state);
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

    bool Check(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Check(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override {
      if (Check(state)) {
        if (ApplyNextStepUntilFound(state, reverting_information, revert_vertex))
          return true;
      }
      return false;
    }


    bool
    EndStepReachable(GameState &state) override {
      bool result = false;
      if (Check(state)) {
        result = EndStepReachableFromNext(state);
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

    bool Check(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Check(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override {
      if (Check(state)) {
        if (ApplyNextStepUntilFound(state, reverting_information, revert_vertex))
          return true;
      }
      return false;
    }


    bool
    EndStepReachable(GameState &state) override {
      bool result = false;
      if (Check(state)) {
        result = EndStepReachableFromNext(state);
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

    bool Check(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Check(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override {
      if (Check(state)) {
        if (ApplyNextStepUntilFound(state, reverting_information, revert_vertex))
          return true;
      }
      return false;
    }


    bool
    EndStepReachable(GameState &state) override {
      bool result = false;
      if (Check(state)) {
        result = EndStepReachableFromNext(state);
      }
      return result;
    }


  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
  };

  class ConditionCheckStep : public SingleSearchStep {
  public:
    explicit ConditionCheckStep(std::unique_ptr<SearchStepsPoint> steps_point);

    bool Check(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Check(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override {
      if (Check(state)) {
        if (ApplyNextStepUntilFound(state, reverting_information, revert_vertex))
          return true;
      }
      return false;
    }


    bool
    EndStepReachable(GameState &state) override {
      bool result = false;
      if (Check(state)) {
        result = EndStepReachableFromNext(state);
      }
      return result;
    }

  private:
    std::unique_ptr<SearchStepsPoint> steps_point_;
  };

  class NegatedConditionCheckStep : public SingleSearchStep {
  public:
    explicit NegatedConditionCheckStep(std::unique_ptr<SearchStepsPoint> steps_point);

    bool Check(GameState &state);

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (Check(state)) {
        RunNextStep(state, applied_modifiers, moves);
      }
    }

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override {
      if (Check(state)) {
        if (ApplyNextStepUntilFound(state, reverting_information, revert_vertex))
          return true;
      }
      return false;
    }


    bool
    EndStepReachable(GameState &state) override {
      bool result = false;
      if (Check(state)) {
        result = EndStepReachableFromNext(state);
      }
      return result;
    }

  private:
    std::unique_ptr<SearchStepsPoint> steps_point_;
  };

  class OffStep : public ModifyingSearchStep {
  public:
    explicit OffStep(piece_id_t piece_id, uint index) : ModifyingSearchStep(index), piece_id_(piece_id) {}

    void Set(GameState &state, piece_id_t &previous_piece) const;

    void Reset(GameState &state, piece_id_t previous_piece) const;

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override;

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override;


    bool
    EndStepReachable(GameState &state) override;

    StepRevertInformation ApplyReversibleAtVertex(GameState &state, vertex_id_t vertex) const override;

    void RevertFromRevertInfo(GameState &state, const StepRevertInformation &) const override;


  private:
    piece_id_t piece_id_;
  };

  class PlayerSwitchStep : public ModifyingSearchStep {
  public:
    explicit PlayerSwitchStep(player_id_t player_id, uint index) : ModifyingSearchStep(index), player_id_(player_id) {}

    void Set(GameState &state, player_id_t &previous_player) const;

    void Reset(GameState &state, player_id_t previous_player) const;

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override;

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override;


    bool
    EndStepReachable(GameState &state) override;

    StepRevertInformation ApplyReversibleAtVertex(GameState &state, vertex_id_t vertex) const override;

    void RevertFromRevertInfo(GameState &state, const StepRevertInformation &info) const override;

  private:
    player_id_t player_id_;
  };

  class AssignmentStep : public ModifyingSearchStep {
  public:
    explicit AssignmentStep(variable_id_t variable_id, std::unique_ptr<ArithmeticOperation> value, uint index)
        : ModifyingSearchStep(index), variable_id_(variable_id), value_(std::move(value)) {}

    bool SetAndCheck(GameState &state, variable_value_t &previous_value) const;

    void Reset(GameState &state, variable_value_t previous_value) const;

    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override;

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override;


    bool
    EndStepReachable(GameState &state) override;

    StepRevertInformation ApplyReversibleAtVertex(GameState &state, vertex_id_t vertex) const override;

    void RevertFromRevertInfo(GameState &state, const StepRevertInformation &info) const override;


  private:
    variable_id_t variable_id_;
    std::unique_ptr<ArithmeticOperation> value_;
  };
}


#endif //RBGGAMEMANAGER_SEARCHSTEP_H
