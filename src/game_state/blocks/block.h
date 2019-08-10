#include <utility>

//
// Created by shrum on 10.06.19.
//

#ifndef RBGGAMEMANAGER_SEARCHSTEP_H
#define RBGGAMEMANAGER_SEARCHSTEP_H


#include "game_description/moves/arithmetic_operation.h"
#include <vector>
#include <stack>
#include <type_traits>
#include <game_description/declarations.h>
#include <stl_extension/vector.h>
#include "utility/resetabble_bitarray_stack.h"
#include "modifying_application.h"

namespace rbg {
  class GameState;

  class BlocksCollection;

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
    const ModifyingApplication *application;
    vertex_id_t vertex;
    name_id_t revert_name;
  };

  class Block {
  public:
    virtual void Run(GameState *) = 0;

    virtual bool RunAndApplyFirst(GameState *state,
                                  std::vector<ActionRevertInfo> *revert_infos,
                                  vertex_id_t application_vertex) = 0;

    virtual bool RunAndFindEnd(GameState *) = 0;
  };

  enum class BranchKind {
    kNone,
    kSingle,
    kMultiple
  };

  class BranchEmpty {
  public:
    static constexpr BranchKind kind = BranchKind::kNone;

    void AddNext(Block *step) { assert(!step && "You cannot set next step to an empty branch."); }

    void RunNext(GameState *) {}

    bool RunNextAndApplyFirst(GameState *,
                              std::vector<ActionRevertInfo> *,
                              vertex_id_t) { return false; }

    bool RunNextAndFindEnd(GameState *) { return true; }
  };

  class BranchSingle {
  public:
    BranchSingle() = default;

    static constexpr BranchKind kind = BranchKind::kSingle;

    void AddNext(Block *step) {
      assert(next_ == nullptr && "Single branch can have only one next block.");
      next_ = step;
    }

    void RunNext(GameState *state) {
      next_->Run(state);
    }

    bool RunNextAndApplyFirst(GameState *state,
                              std::vector<ActionRevertInfo> *revert_infos,
                              vertex_id_t revert_vertex) {
      return next_->RunAndApplyFirst(state, revert_infos, revert_vertex);
    }

    bool RunNextAndFindEnd(GameState *state) {
      return next_->RunAndFindEnd(state);
    }

    Block *next() {
      return next_;
    }

    const Block *next() const {
      return next_;
    }

  private:
    Block *next_;
  };

  class BranchMultiple {
  public:
    BranchMultiple() = default;

    static constexpr BranchKind kind = BranchKind::kMultiple;

    void AddNext(Block *step) { nexts_.push_back(step); }

    void RunNext(GameState *state) {
      for (Block *next_step : nexts_) {
        next_step->Run(state);
      }
    }

    bool RunNextAndApplyFirst(GameState *state,
                              std::vector<ActionRevertInfo> *revert_infos,
                              vertex_id_t revert_vertex) {
      for (Block *next_step : nexts_) {
        if (next_step->RunAndApplyFirst(state, revert_infos, revert_vertex)) {
          return true;
        }
      }
      return false;
    }

    bool RunNextAndFindEnd(GameState *state) {
      for (Block *next_step : nexts_) {
        if (next_step->RunAndFindEnd(state))
          return true;
      }
      return false;
    }

    const std::vector<Block *>& nexts() const {
      return nexts_;
    }

  private:
    std::vector<Block *> nexts_;
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
  struct ActionBlockContentGet;

  template<typename ...BlockElements>
  class ActionsBlockContent {
  };

  template<typename Branch>
  class ActionsBlockContent<Branch> {
  public:
    using branch_type = Branch;

    explicit ActionsBlockContent(Branch branch) : branch_(std::move(branch)) {
      static_assert(IsBranchType<Branch>::value && "The last element of the block should be a branch type.");
    }

    void Run(GameState *state) {
      branch_.RunNext(state);
    }

    bool RunAndApplyFirst(GameState *state,
                          std::vector<ActionRevertInfo> *revert_infos,
                          vertex_id_t revert_vertex) {
      return branch_.RunNextAndApplyFirst(state, revert_infos, revert_vertex);
    }

    bool RunAndFindEnd(GameState *state) {
      return branch_.RunNextAndFindEnd(state);
    }

    branch_type *branch() {
      return &branch_;
    }

    const branch_type *branch() const {
      return &branch_;
    }

  private:
    Branch branch_;
  };

  template<typename Action, typename ...BlockElements>
  class ActionsBlockContent<Action, BlockElements...> {
  public:
    using branch_type = typename ActionsBlockContent<BlockElements...>::branch_type;

    explicit ActionsBlockContent(Action action, BlockElements &&... actions)
        : action_(std::move(action)), next_elements_(std::forward<BlockElements>(actions)...) {}

    /* ------------------------ If the action is of type CHECK, those methods are defined. -------------------------- */
    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kCheck,
        void>::type Run(GameState *state) {
      if (action_.Check(state))
        next_elements_.Run(state);
    }

    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kCheck,
        bool>::type RunAndApplyFirst(GameState *state,
                                     std::vector<ActionRevertInfo> *revert_infos,
                                     vertex_id_t revert_vertex) {
      if (action_.Check(state))
        return next_elements_.RunAndApplyFirst(state, revert_infos, revert_vertex);
      return false;
    }

    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kCheck,
        bool>::type RunAndFindEnd(GameState *state) {
      if (action_.Check(state))
        return next_elements_.RunAndFindEnd(state);
      return false;
    }

    /* ------------------------ If the action is of type APPLICATION, those methods are defined. -------------------- */
    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kApplication,
        bool>::type RunAndApplyFirst(GameState *state,
                                     std::vector<ActionRevertInfo> *revert_infos,
                                     vertex_id_t revert_vertex) {
      typename Action::revert_info_t action_revert_info = action_.Apply(state);
      if (next_elements_.RunAndApplyFirst(state, revert_infos, revert_vertex)) {
        return true;
      }
      action_.Revert(state, action_revert_info);
      return false;
    }

    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kApplication,
        void>::type Run(GameState *state) {
      typename Action::revert_info_t action_revert_info = action_.Apply(state);
      next_elements_.Run(state);
      action_.Revert(state, action_revert_info);
    }

    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kApplication,
        bool>::type RunAndFindEnd(GameState *state) {
      typename Action::revert_info_t action_revert_info = action_.Apply(state);
      if (next_elements_.RunAndFindEnd(state)) {
        action_.Revert(state, action_revert_info);
        return true;
      } else {
        action_.Revert(state, action_revert_info);
        return false;
      }
    }

    /* ------------------------ If the action is of type MODIFIER, those methods are defined. ----------------------- */

    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kModifier,
        void>::type Run(GameState *state) {
      action_.PushVisitedStackAndAddToApplied(state);
      typename Action::revert_info_t action_revert_info = action_.Apply(state);
      next_elements_.Run(state);
      action_.PopVisitedStackAndAddToApplied(state);
      action_.Revert(state, action_revert_info);
    }

    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kModifier,
        bool>::type RunAndApplyFirst(GameState *state,
                                     std::vector<ActionRevertInfo> *revert_infos,
                                     vertex_id_t revert_vertex) {
      action_.PushVisitedStackAndAddToApplied(state);
      typename Action::revert_info_t action_revert_info = action_.Apply(state);
      revert_infos->push_back(ActionRevertInfo{&action_, revert_vertex, action_revert_info});
      if (next_elements_.RunAndApplyFirst(state, revert_infos, action_.modified_position(state))) {
        return true;
      }
      action_.Revert(state, action_revert_info);
      action_.PopVisitedStackAndAddToApplied(state);
      revert_infos->pop_back();
      return false;
    }

    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kModifier,
        bool>::type RunAndFindEnd(GameState *state) {
      action_.PushVisitedStackAndAddToApplied(state);
      typename Action::revert_info_t action_revert_info = action_.Apply(state);
      if (next_elements_.RunAndFindEnd(state)) {
        action_.Revert(state, action_revert_info);
        action_.PopVisitedStackAndAddToApplied(state);
        return true;
      } else {
        action_.Revert(state, action_revert_info);
        action_.PopVisitedStackAndAddToApplied(state);
        return false;
      }
    }

    /* ------------------------ If the action is of type SWITCH, those methods are defined. ----------------------- */

    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kSwitch,
        void>::type Run(GameState *state) {
      action_.PushVisitedStackAndAddToApplied(state);
      typename Action::revert_info_t action_revert_info = action_.Apply(state);
      action_.RegisterMove(state);
      action_.PopVisitedStackAndAddToApplied(state);
      action_.Revert(state, action_revert_info);
    }


    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kSwitch,
        bool>::type RunAndApplyFirst(GameState *state,
                                     std::vector<ActionRevertInfo> *revert_infos,
                                     vertex_id_t) {
      action_.PushVisitedStackAndAddToApplied(state);
      typename Action::revert_info_t action_revert_info = action_.Apply(state);
      revert_infos->push_back(ActionRevertInfo{&action_, action_.modified_position(state), action_revert_info});
      return true;
    }

    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kSwitch,
        bool>::type RunAndFindEnd(GameState *state) {
      action_.PushVisitedStackAndAddToApplied(state);
      typename Action::revert_info_t action_revert_info = action_.Apply(state);
      if (next_elements_.RunAndFindEnd(state)) {
        action_.Revert(state, action_revert_info);
        action_.PopVisitedStackAndAddToApplied(state);
        return true;
      } else {
        action_.Revert(state, action_revert_info);
        action_.PopVisitedStackAndAddToApplied(state);
        return false;
      }
    }

     /* ------------------------ If the action is of type SPECIAL, those methods are defined. ----------------------- */
     /* This kind of action gives tha action full control of what is happening in each call. */

    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kSpecial,
        void>::type Run(GameState *state) {
      action_.Run(state, &next_elements_);
    }


    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kSpecial,
        bool>::type RunAndApplyFirst(GameState *state,
                                     std::vector<ActionRevertInfo> *revert_infos,
                                     vertex_id_t revert_vertex) {
      return action_.RunAndApplyFirst(state, revert_infos, revert_vertex, &next_elements_);
    }

    template<ActionTypeTrait ActionKind = Action::kind>
    typename std::enable_if<ActionKind == ActionTypeTrait::kSpecial,
        bool>::type RunAndFindEnd(GameState *state) {
      return action_.RunAndFindEnd(state, &next_elements_);
    }

    /* ------------------------ Those functions are defined always in a BlockContent -------------------------------- */
    Action *action() {
      return &action_;
    };

    const Action *action() const {
      return &action_;
    }

    branch_type *branch() {
      return next_elements_.branch();
    }

    ActionsBlockContent<BlockElements...> *next_elements() {
      return &next_elements_;
    }

    const ActionsBlockContent<BlockElements...> *next_elements() const {
      return &next_elements_;
    }

  protected:
    Action action_;
    ActionsBlockContent<BlockElements...> next_elements_;
  };

  template<typename BlockContent>
  class ActionsBlockPointer : public Block {
  public:
    explicit ActionsBlockPointer(BlockContent *elements) : elements_(elements) {}

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

    const BlockContent &content() const {
      return *elements_;
    }

  private:
    BlockContent *elements_;
  };


  template<typename First, typename ...Rest>
  struct ActionBlockContentGet<0, ActionsBlockContent<First, Rest ...>> {
    using FirstElementType = First;
    using SubBlockType = ActionsBlockContent<First, Rest...>;

    static auto GetSubContent(ActionsBlockContent<First, Rest...> *block_content) {
      return block_content;
    }
  };

  template<std::size_t I, typename First, typename ...Rest>
  struct ActionBlockContentGet<I, ActionsBlockContent<First, Rest...>> {
    using FirstElementType = typename ActionBlockContentGet<I - 1, ActionsBlockContent<Rest ...>>::FirstElementType;
    using SubBlockType  = typename ActionBlockContentGet<I - 1, ActionsBlockContent<Rest ...>>::SubBlockType;

    static auto GetSubContent(ActionsBlockContent<First, Rest...> *block_content) {
      return ActionBlockContentGet<I - 1, ActionsBlockContent<Rest ...>>::GetSubContent(block_content->next_elements());
    }
  };

  template<typename ...BlockElements>
  class ActionsBlock : public Block {
  public:
    explicit ActionsBlock(BlockElements &&... elements) : elements_(std::forward<BlockElements>(elements)...) {}

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

    template<size_t I>
    auto GetSubAbstractBlock() {
      auto sub_block_pointer = ActionBlockContentGet<I, ActionsBlockContent<BlockElements...>>::GetSubContent(
          &elements_);
      sub_pointers_.push_back(
          std::make_unique<ActionsBlockPointer<typename ActionBlockContentGet<I, ActionsBlockContent<BlockElements...>>::SubBlockType>>(
              sub_block_pointer));
      return sub_pointers_.back().get();
    }

    template<size_t I>
    auto GetAction() {
      return ActionBlockContentGet<I, ActionsBlockContent<BlockElements...>>::GetSubContent(&elements_)->action();
    }

    ActionsBlockContent<BlockElements...> &content() {
      return elements_;
    }

    const ActionsBlockContent<BlockElements...> &content() const {
      return elements_;
    }

  private:
    ActionsBlockContent<BlockElements...> elements_;
    std::vector<std::unique_ptr<Block>> sub_pointers_;
  };

  template<typename ...BlockElements>
  std::unique_ptr<ActionsBlock<BlockElements...>> CreateBlockUniquePtr(BlockElements... elements) {
    return std::make_unique<ActionsBlock<BlockElements...>>(std::move(elements)...);
  }
}

#endif //RBGGAMEMANAGER_SEARCHSTEP_H
