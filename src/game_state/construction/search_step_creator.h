//
// Created by shrum on 23.06.19.
//

#ifndef RBGGAMEMANAGER_SEARCH_STEP_CREATOR_H
#define RBGGAMEMANAGER_SEARCH_STEP_CREATOR_H

#include "game_state/construction/moves/moves_visitor.h"
#include "graph_creator.h"
#include <unordered_map>
#include <game_state/search_steps/block.h>

namespace rbg {

  class SearchStepsCollection {
  public:
    SearchStepsCollection(uint visited_checks_count, uint board_size)
        : visited_info_stack_(std::make_unique<ResettableBitArrayStack>(visited_checks_count * board_size)),
          visited_checks_count_(visited_checks_count) {}

    uint AddSearchStep(std::unique_ptr<AbstractBlock> step) {
      searchsteps_.emplace_back(std::move(step));
      return searchsteps_.size() - 1;
    }

    AbstractBlock *operator[](uint index) {
      return searchsteps_[index].get();
    }

    ResetableBitArrayStackChunk GetBitArrayChunk(uint index) {
      return {visited_info_stack_.get(),
              index * visited_info_stack_->bit_array_size() / visited_checks_count_,
              visited_info_stack_->bit_array_size() / visited_checks_count_};
    }

    ResettableBitArrayStack &stack() {
      return *visited_info_stack_;
    }

    void RegisterModifier(uint modifier_index, const ModifyingApplication* application) {
      modifiers_[modifier_index] = application;
    }

    void RegisterSwitch(uint modifier_index, AbstractBlock* block) {
      switches_[modifier_index] = block;
    }

    const ModifyingApplication *Modifier(uint modifier_index) {
      return modifiers_.at(modifier_index);
    }

    AbstractBlock *Switch(uint modifier_index) {
      return switches_.at(modifier_index);
    }

  private:
    std::vector<std::unique_ptr<AbstractBlock>> searchsteps_;
    std::unique_ptr<ResettableBitArrayStack> visited_info_stack_;
    uint visited_checks_count_;

    std::unordered_map<uint, const ModifyingApplication*> modifiers_;
    std::unordered_map<uint, AbstractBlock*> switches_;

    friend class NewVisitedCheckLayerApplication;
    friend class PlayerSwitchApplication;
  };

  class SearchStepsPoint {
  public:
    explicit SearchStepsPoint(SearchStepsCollection &parent_collection, AbstractBlock *initial = nullptr) :
        parent_(&parent_collection), current_(initial), initial_(initial) {}

    void SetInitial(uint index) {
      initial_ = (*parent_)[index];
    }

    AbstractBlock *current() {
      return current_;
    }

    void set_current(AbstractBlock *step) {
      current_ = step;
    }

    void SetParent(SearchStepsCollection *parent) {
      parent_ = parent;
    }

    void reset() {
      current_ = initial_;
    }
  private:
    SearchStepsCollection *parent_;
    AbstractBlock *current_;
    AbstractBlock *initial_;
  };

  struct SearchStepsInformation {
    SearchStepsCollection collection;
    SearchStepsPoint current_position;
  };

  SearchStepsInformation CreateSearchSteps(const VisitedChecksNfa &nfa, const Declarations &declarations);
}


#endif //RBGGAMEMANAGER_SEARCH_STEP_CREATOR_H
