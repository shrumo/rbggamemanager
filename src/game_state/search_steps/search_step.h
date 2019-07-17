#include <utility>

//
// Created by shrum on 10.06.19.
//

#ifndef RBGGAMEMANAGER_SEARCHSTEP_H
#define RBGGAMEMANAGER_SEARCHSTEP_H


#include "arithmetic_operation.h"
#include <vector>
#include <stack>
#include <type_traits>
#include <game_state/declarations.h>
#include <stl_extension/vector.h>
#include "stl_extension/resetabblebitarraystack.h"
#include "public_actions.h"

namespace rbg {
  class GameState;
  class SearchStepsCollection;
  class SearchStepsPoint;
  struct SearchStepsInformation;
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
    virtual void RunNextStep(GameState &state,
                     std::vector<ModifierApplication> &applied_modifiers,
                     std::vector<std::vector<ModifierApplication>> &moves) {
      next_->Run(state, applied_modifiers, moves);
    }

    virtual bool ApplyNextStepUntilFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                                 vertex_id_t revert_vertex) {
      return next_->ApplyFirstFound(state, reverting_information, revert_vertex);
    }


    virtual bool EndStepReachableFromNext(GameState &state) {
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

  struct ActionRevertInfo {
    const ModifyingApplication* application;
    vertex_id_t vertex;
    name_id_t revert_name;
  };

  class AbstractBlock {
  public:
    virtual void Run(GameState*) = 0;
    virtual bool RunAndApplyFirst(GameState *state,
        std::vector<ActionRevertInfo> *revert_infos,
        vertex_id_t application_vertex) = 0;
    virtual bool RunAndFindEnd(GameState*) = 0;
    virtual void AddNextBlock(AbstractBlock * step) = 0;
  };

  enum class BranchTypeTrait {
    NONE,
    SINGLE,
    MULTIPLE
  };

 class BranchEmpty {
     public:
    static constexpr BranchTypeTrait type = BranchTypeTrait :: NONE;
    void AddNext(AbstractBlock *step) { assert(!step && "You cannot set next step to an empty branch."); }
    void RunNext(GameState*) {} __attribute__((always_inline))
     bool RunNextAndApplyFirst(GameState*,
        std::vector<ActionRevertInfo>*,
        vertex_id_t) __attribute__((always_inline))  __attribute__((always_inline)){ return false; }
     bool RunNextAndFindEnd(GameState*) __attribute__((always_inline)){ return true; }
  };

  class BranchSingle {
  public:
    BranchSingle() = default;

    static constexpr BranchTypeTrait type = BranchTypeTrait :: SINGLE;
    void AddNext(AbstractBlock *step) { assert(next_ == nullptr && "Single branch can have only one next block."); next_ = step; }
     void RunNext(GameState *state) __attribute__((always_inline)){
      next_->Run(state);
    }

     bool RunNextAndApplyFirst(GameState *state,
        std::vector<ActionRevertInfo> *revert_infos,
        vertex_id_t revert_vertex) __attribute__((always_inline)){
      return next_->RunAndApplyFirst(state, revert_infos, revert_vertex);
    }

     bool RunNextAndFindEnd(GameState *state)__attribute__((always_inline)) {
      return next_->RunAndFindEnd(state);
    }
  private:
    AbstractBlock *next_;
  };

  class BranchMultiple {
  public:
    BranchMultiple() = default;

    static constexpr BranchTypeTrait type = BranchTypeTrait :: MULTIPLE;
    void AddNext(AbstractBlock *step) { next_steps_.push_back(step); }
     void RunNext(GameState *state) __attribute__((always_inline)){
      for(AbstractBlock* next_step : next_steps_) {
      next_step->Run(state);
      }
    }
     bool RunNextAndApplyFirst(GameState *state,
        std::vector<ActionRevertInfo> *revert_infos,
        vertex_id_t revert_vertex) __attribute__((always_inline)){
      for(AbstractBlock* next_step : next_steps_) {
      if(next_step->RunAndApplyFirst(state, revert_infos, revert_vertex)) {
      return true;
      }
    }
   return false;
    }
     bool RunNextAndFindEnd(GameState *state) __attribute__((always_inline))
    {
       for(AbstractBlock* next_step : next_steps_) {
         if(next_step->RunAndFindEnd(state))
           return true;
       }
       return false;
    }
  private:
    std::vector<AbstractBlock*> next_steps_;
  };

  template<std::size_t I, typename T>
  struct BlockContentGet;

  template<typename ...BlockElements>
  class BlockContent {};

  template<typename BranchType>
  class BlockContent<BranchType> {
  public:
    explicit BlockContent(BranchType branch) : branch_(std::move(branch)) {}

     void Run(GameState* state) __attribute__((always_inline)){
      branch_.RunNext(state);
    }

     bool RunAndApplyFirst(GameState *state,
        std::vector<ActionRevertInfo> *revert_infos,
        vertex_id_t revert_vertex) __attribute__((always_inline)){
      return branch_.RunNextAndApplyFirst(state, revert_infos, revert_vertex);
    }

     bool RunAndFindEnd(GameState* state) __attribute__((always_inline)){
      return branch_.RunNextAndFindEnd(state);
    }

     void AddNextBlock(AbstractBlock *step) __attribute__((always_inline)){ branch_.AddNext(step); }

  private:
    BranchType branch_;
  };

  template<typename ActionType, typename ...BlockElements>
  class BlockContent<ActionType, BlockElements...> {
  public:
    explicit BlockContent(ActionType action, BlockElements&&... actions)
    : action_(std::move(action)), next_elements_(std::forward<BlockElements>(actions)...)
    {}

    template<typename ActionTypeCopy = ActionType>
     __attribute__((always_inline)) typename std::enable_if<ActionTypeCopy::type == ActionTypeTrait :: CHECK,
    void>::type Run(GameState* state) {
      if(action_.Check(state))
        next_elements_.Run(state);
    }

    template<typename ActionTypeCopy = ActionType>
    __attribute__((always_inline)) typename std::enable_if<ActionTypeCopy::type == ActionTypeTrait :: CHECK,
    bool>::type RunAndApplyFirst(GameState *state,
        std::vector<ActionRevertInfo> *revert_infos,
        vertex_id_t revert_vertex) {
       if(action_.Check(state))
        return next_elements_.RunAndApplyFirst(state, revert_infos, revert_vertex);
       return false;
    }

    template<typename ActionTypeCopy = ActionType>
    __attribute__((always_inline)) typename std::enable_if<ActionTypeCopy::type == ActionTypeTrait :: CHECK,
    bool>::type RunAndFindEnd(GameState* state) {
      if(action_.Check(state))
        return next_elements_.RunAndFindEnd(state);
      return false;
    }

    template<typename ActionTypeCopy = ActionType>
     __attribute__((always_inline))typename std::enable_if<ActionTypeCopy::type == ActionTypeTrait :: MODIFIER,
    void>::type Run(GameState* state) {
      action_.PushVisitedStackAndAddToApplied(state);
      typename ActionType::revert_info_t action_revert_info = action_.Apply(state);
      next_elements_.Run(state);
      action_.PopVisitedStackAndAddToApplied(state);
      action_.Revert(state, action_revert_info);
    }

    template<typename ActionTypeCopy = ActionType>
     __attribute__((always_inline))typename std::enable_if<ActionTypeCopy::type == ActionTypeTrait :: MODIFIER,
    bool>::type RunAndApplyFirst(GameState *state,
        std::vector<ActionRevertInfo> *revert_infos,
        vertex_id_t revert_vertex) {
      action_.PushVisitedStackAndAddToApplied(state);
      typename ActionType::revert_info_t action_revert_info = action_.Apply(state);
      revert_infos->push_back(ActionRevertInfo{&action_, action_.weird_current_state_pos(state), action_revert_info});
      if(next_elements_.RunAndApplyFirst(state, revert_infos, revert_vertex)) {
        return true;
      }
      action_.PopVisitedStackAndAddToApplied(state);
      revert_infos->pop_back();
      return false;
    }

    template<typename ActionTypeCopy = ActionType>
     __attribute__((always_inline))typename std::enable_if<ActionTypeCopy::type == ActionTypeTrait :: APPLICATION,
    bool>::type RunAndApplyFirst(GameState *state,
        std::vector<ActionRevertInfo> *revert_infos,
        vertex_id_t revert_vertex) {
      typename ActionType::revert_info_t action_revert_info = action_.Apply(state);
      if(next_elements_.RunAndApplyFirst(state,  revert_infos, revert_vertex)) {
        return true;
      }
      action_.Revert(state, action_revert_info);
      return false;
    }

    template<typename ActionTypeCopy = ActionType>
     __attribute__((always_inline))typename std::enable_if<ActionTypeCopy::type == ActionTypeTrait :: APPLICATION,
    void>::type Run(GameState* state) {
      typename ActionType::revert_info_t action_revert_info = action_.Apply(state);
      next_elements_.Run(state);
      action_.Revert(state, action_revert_info);
    }

    template<typename ActionTypeCopy = ActionType>
     __attribute__((always_inline))typename std::enable_if<ActionTypeCopy::type == ActionTypeTrait :: APPLICATION,
    bool>::type RunAndFindEnd(GameState* state) {
      typename ActionType::revert_info_t action_revert_info = action_.Apply(state);
    if(next_elements_.RunAndFindEnd(state)) {
      action_.Revert(state, action_revert_info);
      return true;
    } else {
      action_.Revert(state, action_revert_info);
      return false;
    }
    }

    template<typename ActionTypeCopy = ActionType>
     __attribute__((always_inline))typename std::enable_if<ActionTypeCopy::type == ActionTypeTrait :: SWITCH,
    void>::type Run(GameState* state) {
      action_.PushVisitedStackAndAddToApplied(state);
      typename ActionType::revert_info_t action_revert_info = action_.Apply(state);
      action_.RegisterMove(state);
      action_.PopVisitedStackAndAddToApplied(state);
      action_.Revert(state, action_revert_info);
    }

    template<typename ActionTypeCopy = ActionType>
     __attribute__((always_inline))typename std::enable_if<ActionTypeCopy::type == ActionTypeTrait :: SWITCH,
    bool>::type RunAndApplyFirst(GameState *state,
        std::vector<ActionRevertInfo> *revert_infos,
        vertex_id_t revert_vertex) {
      action_.PushVisitedStackAndAddToApplied(state);
      typename ActionType::revert_info_t action_revert_info = action_.Apply(state);
      revert_infos->push_back(ActionRevertInfo{&action_, action_.weird_current_state_pos(state), action_revert_info});
      return true;
    }

    template<typename ActionTypeCopy = ActionType>
     __attribute__((always_inline))typename std::enable_if< ActionTypeCopy::type == ActionTypeTrait :: MODIFIER ||
    ActionTypeCopy::type == ActionTypeTrait :: SWITCH,
    bool>::type RunAndFindEnd(GameState* state) {
      action_.PushVisitedStackAndAddToApplied(state);
    typename ActionType::revert_info_t action_revert_info = action_.Apply(state);
    if(next_elements_.RunAndFindEnd(state)) {
      action_.Revert(state, action_revert_info);
      action_.PopVisitedStackAndAddToApplied(state);
      return true;
    } else {
      action_.Revert(state, action_revert_info);
      action_.PopVisitedStackAndAddToApplied(state);
      return false;
    }
    }

    void AddNextBlock(AbstractBlock * step) {
      next_elements_.AddNextBlock(step);
    }

    ActionType* action(){
      return &action_;
    };

    const ActionType* action() const {
      return &action_;
    }

    BlockContent<BlockElements...>* next_elements() {
      return &next_elements_;
    }

    const BlockContent<BlockElements...>* next_elements() const {
      return &next_elements_;
    }



  protected:
    ActionType action_;
    BlockContent<BlockElements...> next_elements_;
  };


  template<typename First, typename ...Rest>
  struct BlockContentGet<0, BlockContent<First, Rest ...>> {
    static auto get(BlockContent<First, Rest...>* block_content) {
      return block_content;
    }
  };

  template<std::size_t I, typename First, typename ...Rest>
  struct BlockContentGet<I, BlockContent<First, Rest...>>{
    static auto get(BlockContent<First, Rest...>* block_content) {
      return BlockContentGet<I - 1, BlockContent<Rest ...>>::get(block_content->next_elements());
    }
  };

  template<typename ...BlockElements>
  class BlockPointer : public AbstractBlock {
      public:
    explicit BlockPointer(BlockContent<BlockElements...>* elements) : elements_(elements) {}

    void Run(GameState *state) override {
      elements_->Run(state);
    }

    bool RunAndApplyFirst(GameState *state,
        std::vector<ActionRevertInfo> *revert_infos,
        vertex_id_t revert_vertex) override {
      return elements_->RunAndApplyFirst(state, revert_infos, revert_vertex);
    }

    bool RunAndFindEnd(GameState *state) override {
      return elements_->RunAndFindEnd(state);
    }

    void AddNextBlock(AbstractBlock *step) override {
      elements_->AddNextBlock(step);
    }

    const BlockContent<BlockElements...>& content() const {
      return *elements_;
    }
  private:
    BlockContent<BlockElements...> *elements_;
  };

  template<typename ...BlockElements>
  class Block : public AbstractBlock {
  public:
    explicit Block(BlockElements&&... elements) : elements_(std::forward<BlockElements>(elements)...) {}

    void Run(GameState *state) override {
      elements_.Run(state);
    }

    bool RunAndApplyFirst(GameState *state,
        std::vector<ActionRevertInfo> *revert_infos,
        vertex_id_t revert_vertex) override {
      return elements_.RunAndApplyFirst(state, revert_infos, revert_vertex);
    }

    bool RunAndFindEnd(GameState *state) override {
      return elements_.RunAndFindEnd(state);
    }

    void AddNextBlock(AbstractBlock *step) override {
      elements_.AddNextBlock(step);
    }

    template<size_t I>
    auto GetSubBlockPointer()
    {
      return BlockContentGet<I, BlockContent<BlockElements...>>::get(&elements_);
    }

    const BlockContent<BlockElements...>& content() const {
      return elements_;
    }
  private:
    BlockContent<BlockElements...> elements_;
  };





  template<typename ...Actions>
  class WeirdSingleStep : public SingleSearchStep {
  public:

     void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
        RunNextStep(state, applied_modifiers, moves);

    }

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override {
        if (ApplyNextStepUntilFound(state, reverting_information, revert_vertex))
          return true;

      return false;
    }


    bool
    EndStepReachable(GameState &state) override {
      bool result = false;
        result = EndStepReachableFromNext(state);
      return result;
    }

  private:
  };

  template<typename Action, typename ...Actions>
  class WeirdSingleStep<Action, Actions...> : public SingleSearchStep {
  public:
    WeirdSingleStep(Action action, Actions... actions)
      : action_(std::move(action)), next_actions_(std::move(actions)...) {}

    typename std::enable_if<Action::type == ActionTypeTrait::CHECK>::type Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
             std::vector<std::vector<ModifierApplication>> &moves) override {
      if (action_.Check(&state))
        next_actions_.Run(state, applied_modifiers, moves);
    }

    bool
    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
                    vertex_id_t revert_vertex) override {
      if (action_.Check(&state))
        return next_actions_.ApplyFirstFound(state, reverting_information, revert_vertex);
      return false;
    }

    bool
    EndStepReachable(GameState &state) final {
      if (action_.Check(&state))
        return next_actions_.EndStepReachable(state);
      return false;
    }

    void AddNextSearchStep(SearchStep *next) override {
      next_actions_.AddNextSearchStep(next);
    }

  private:
    Action action_;
    WeirdSingleStep<Actions...> next_actions_;
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
    explicit ShiftStep(shift_edge_id edge_id) : edge_id_(edge_id) {}

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
    shift_edge_id edge_id_;
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

//  class ConditionCheckStep : public SingleSearchStep {
//  public:
//    explicit ConditionCheckStep(std::unique_ptr<SearchStepsPoint> steps_point);
//
//    bool Check(GameState &state);
//
//    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
//             std::vector<std::vector<ModifierApplication>> &moves) override {
//      if (Check(state)) {
//        RunNextStep(state, applied_modifiers, moves);
//      }
//    }
//
//    bool
//    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
//                    vertex_id_t revert_vertex) override {
//      if (Check(state)) {
//        if (ApplyNextStepUntilFound(state, reverting_information, revert_vertex))
//          return true;
//      }
//      return false;
//    }
//
//
//    bool
//    EndStepReachable(GameState &state) override {
//      bool result = false;
//      if (Check(state)) {
//        result = EndStepReachableFromNext(state);
//      }
//      return result;
//    }
//
//  private:
//    std::unique_ptr<SearchStepsPoint> steps_point_;
//  };
//
//  class NegatedConditionCheckStep : public SingleSearchStep {
//  public:
//    explicit NegatedConditionCheckStep(std::unique_ptr<SearchStepsPoint> steps_point);
//
//    bool Check(GameState &state);
//
//    void Run(GameState &state, std::vector<ModifierApplication> &applied_modifiers,
//             std::vector<std::vector<ModifierApplication>> &moves) override {
//      if (Check(state)) {
//        RunNextStep(state, applied_modifiers, moves);
//      }
//    }
//
//    bool
//    ApplyFirstFound(GameState &state, std::vector<StepRevertInformation> &reverting_information,
//                    vertex_id_t revert_vertex) override {
//      if (Check(state)) {
//        if (ApplyNextStepUntilFound(state, reverting_information, revert_vertex))
//          return true;
//      }
//      return false;
//    }
//
//
//    bool
//    EndStepReachable(GameState &state) override {
//      bool result = false;
//      if (Check(state)) {
//        result = EndStepReachableFromNext(state);
//      }
//      return result;
//    }
//
//  private:
//    std::unique_ptr<SearchStepsPoint> steps_point_;
//  };

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
