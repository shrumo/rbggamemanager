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
        : current_(nullptr),
          visited_info_stack_(std::make_unique<ResettableBitArrayStack>(visited_checks_count * board_size)),
          visited_checks_count_(visited_checks_count) {}

    uint AddSearchStep(std::unique_ptr<SearchStep> step) {
      searchsteps_.emplace_back(std::move(step));
      return searchsteps_.size() - 1;
    }

    void SetInitial(uint index) {
      current_ = searchsteps_[index].get();
    }

    SearchStep *operator[](uint index) {
      return searchsteps_[index].get();
    }

    SearchStep *current() {
      return current_;
    }

    void set_current(SearchStep *step) {
      current_ = step;
    }

    ResetableBitArrayStackChunk GetBitArrayChunk(uint index) {
      return {visited_info_stack_.get(),
              index * visited_info_stack_->bit_array_size() / visited_checks_count_,
              visited_info_stack_->bit_array_size() / visited_checks_count_};
    }

    ResettableBitArrayStack &stack() {
      return *visited_info_stack_;
    }

  private:
    std::vector<std::unique_ptr<SearchStep>> searchsteps_;
    SearchStep *current_;
    std::unique_ptr<ResettableBitArrayStack> visited_info_stack_;
    uint visited_checks_count_;

    friend class PlayerSwitchStep;
  };

  class SearchStepsInformation : public SearchStepsCollection {
  public:
    SearchStepsInformation(uint visited_checks_count, uint board_size)
        : SearchStepsCollection(visited_checks_count, board_size) {}

    void RegisterModifier(uint modifier_index, uint step_index) {
      modifiers_[modifier_index] = dynamic_cast<ModifyingSearchStep *>((*this)[step_index]);
    }

    ModifyingSearchStep *Modifier(uint modifier_index) {
      return modifiers_[modifier_index];
    }

  private:
    std::unordered_map<uint, ModifyingSearchStep *> modifiers_;
  };

  SearchStepsInformation CreateSearchSteps(const NfaWithVisitedChecks &nfa, const Declarations &declarations);
}


#endif //RBGGAMEMANAGER_SEARCH_STEP_CREATOR_H
