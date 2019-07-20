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

    AbstractBlock* next() {
      return next_;
    }

    const AbstractBlock* next() const {
      return next_;
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

  template<typename T>
  struct IsBranchType {
    static constexpr bool value = false;
  };

  template<>
  struct IsBranchType<BranchEmpty> {
    static constexpr bool value = true;
  };

  template<>
  struct IsBranchType<BranchSingle> {
    static constexpr bool value = true;
  };

  template<>
  struct IsBranchType<BranchMultiple> {
    static constexpr bool value = true;
  };

  template<std::size_t I, typename T>
  struct BlockContentGet;

  template<typename ...BlockElements>
  class BlockContent {};

  template<typename BranchType>
  class BlockContent<BranchType> {
  public:
    explicit BlockContent(BranchType branch) : branch_(std::move(branch)) {
      static_assert(IsBranchType<BranchType>::value && "The last element of the block should be a branch type.");
    }

     __attribute__((always_inline)) void Run(GameState* state) {
      branch_.RunNext(state);
    }

     __attribute__((always_inline)) bool RunAndApplyFirst(GameState *state,
        std::vector<ActionRevertInfo> *revert_infos,
        vertex_id_t revert_vertex) {
      return branch_.RunNextAndApplyFirst(state, revert_infos, revert_vertex);
    }

    __attribute__((always_inline)) bool RunAndFindEnd(GameState* state) {
      return branch_.RunNextAndFindEnd(state);
    }

    __attribute__((always_inline)) void AddNextBlock(AbstractBlock *step) { branch_.AddNext(step); }

    BranchType* branch() {
      return &branch_;
    }

    const BranchType* branch() const {
      return &branch_;
    }

  private:
    BranchType branch_;
  };

  template<typename ActionType, typename ...BlockElements>
  class BlockContent<ActionType, BlockElements...> {
  public:
    explicit BlockContent(ActionType action, BlockElements&&... actions)
    : action_(std::move(action)), next_elements_(std::forward<BlockElements>(actions)...)
    {}

    // If the action is of type CHECK, those methods are defined.
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

    // If the action is of type APPLICATION, those methods are defined.
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

    // If the action is of type MODIFIER, those methods are defined.
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

    // If the action is of type MODIFIER, those methods are defined.
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
        vertex_id_t) {
      action_.PushVisitedStackAndAddToApplied(state);
      typename ActionType::revert_info_t action_revert_info = action_.Apply(state);
      revert_infos->push_back(ActionRevertInfo{&action_, action_.weird_current_state_pos(state), action_revert_info});
      return true;
    }

    // If the action is of type MODIFIER or of type SWITCH, those methods are defined.
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
    using FirstElementType = First;
    using SubBlockType = BlockContent<First, Rest...>;
    static auto GetSubContent(BlockContent<First, Rest...> *block_content) {
      return block_content;
    }
  };

  template<std::size_t I, typename First, typename ...Rest>
  struct BlockContentGet<I, BlockContent<First, Rest...>>{
    using FirstElementType = typename BlockContentGet<I - 1, BlockContent<Rest ...>>::FirstElementType ;
    using SubBlockType  = typename BlockContentGet<I - 1, BlockContent<Rest ...>>::SubBlockType ;
    static auto GetSubContent(BlockContent<First, Rest...> *block_content) {
      return BlockContentGet<I - 1, BlockContent<Rest ...>>::GetSubContent(block_content->next_elements());
    }
  };

  template<typename BlockContent>
  class BlockPointer : public AbstractBlock {
      public:
    explicit BlockPointer(BlockContent* elements) : elements_(elements) {}

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

    void AddNextBlock(AbstractBlock *) override {
      assert(false);
    }

    const BlockContent& content() const {
      return *elements_;
    }
  private:
    BlockContent *elements_;
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
    auto GetSubAbstractBlock()
    {
      auto sub_block_pointer = BlockContentGet<I, BlockContent<BlockElements...>>::GetSubContent(&elements_);
      sub_pointers_.push_back(std::make_unique<BlockPointer<typename BlockContentGet<I, BlockContent<BlockElements...>>::SubBlockType>>(sub_block_pointer));
      return sub_pointers_.back().get();
    }

    template<size_t I>
    auto GetAction()
    {
      return BlockContentGet<I, BlockContent<BlockElements...>>::GetSubContent(&elements_)->action();
    }

    const BlockContent<BlockElements...>& content() const {
      return elements_;
    }
  private:
    BlockContent<BlockElements...> elements_;
    std::vector<std::unique_ptr<AbstractBlock>> sub_pointers_;
  };
}

#endif //RBGGAMEMANAGER_SEARCHSTEP_H
