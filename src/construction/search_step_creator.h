//
// Created by shrum on 23.06.19.
//

#ifndef RBGGAMEMANAGER_SEARCH_STEP_CREATOR_H
#define RBGGAMEMANAGER_SEARCH_STEP_CREATOR_H

#include "moves/moves_visitor.h"
#include "graph_creator.h"
#include <unordered_map>

namespace rbg {

  class SearchStepsCollection {
  public:
    SearchStepsCollection(uint visited_checks_count, uint board_size)
        : visited_info_stack_(std::make_unique<ResettableBitArrayStack>(visited_checks_count * board_size)),
          visited_checks_count_(visited_checks_count) {}

    uint AddSearchStep(std::unique_ptr<SearchStep> step) {
      searchsteps_.emplace_back(std::move(step));
      return searchsteps_.size() - 1;
    }

    SearchStep *operator[](uint index) {
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

    void RegisterModifier(uint modifier_index, uint step_index) {
      modifiers_[modifier_index] = dynamic_cast<ModifyingSearchStep *>((*this)[step_index]);
    }

    ModifyingSearchStep *Modifier(uint modifier_index) {
      return modifiers_[modifier_index];
    }

  private:
    std::vector<std::unique_ptr<SearchStep>> searchsteps_;
    std::unique_ptr<ResettableBitArrayStack> visited_info_stack_;
    uint visited_checks_count_;

    std::unordered_map<uint, ModifyingSearchStep *> modifiers_;

    friend class PlayerSwitchStep;
  };

  class SearchStepsPoint {
  public:
    explicit SearchStepsPoint(SearchStepsCollection &parent_collection, SearchStep *current = nullptr) :
        parent_(parent_collection), current_(current) {}

    void SetInitial(uint index) {
      current_ = parent_[index];
    }

    SearchStep *current() {
      return current_;
    }

    void set_current(SearchStep *step) {
      current_ = step;
    }

  private:
    SearchStepsCollection &parent_;
    SearchStep *current_;
  };

  struct SearchStepsInformation {
    SearchStepsCollection collection;
    SearchStepsPoint current_position;
  };

  SearchStepsInformation CreateSearchSteps(const VisitedChecksNfa &nfa, const Declarations &declarations);
}


#endif //RBGGAMEMANAGER_SEARCH_STEP_CREATOR_H
