//
// Created by shrum on 23.06.19.
//

#ifndef RBGGAMEMANAGER_SEARCHSTEP_CREATOR_H
#define RBGGAMEMANAGER_SEARCHSTEP_CREATOR_H

#include "moves/moves_visitor.h"
#include "graph_creator.h"

namespace rbg {
  class SearchstepsCollection {
  public:
    SearchstepsCollection(uint visited_checks_count, uint board_size)
        : initial_(nullptr),
          visited_info_stack_(visited_checks_count * board_size),
          visited_checks_count_(visited_checks_count) {}

    uint AddSearchStep(std::unique_ptr<SearchStep> step) {
      searchsteps_.emplace_back(std::move(step));
      return searchsteps_.size() - 1;
    }

    void SetInitial(uint index) {
      initial_ = searchsteps_[index].get();
    }

    SearchStep *operator[](uint index) {
      return searchsteps_[index].get();
    }

    SearchStep *initial() {
      return initial_;
    }

    ResetableBitArrayStackChunk GetBitArrayChunk(uint index) {
      return {visited_info_stack_,
              index * visited_info_stack_.bit_array_size() / visited_checks_count_,
              visited_info_stack_.bit_array_size() / visited_checks_count_};
    }

  private:
    std::vector<std::unique_ptr<SearchStep>> searchsteps_;
    SearchStep *initial_;
    ResettableBitArrayStack visited_info_stack_;
    uint visited_checks_count_;
  };

  SearchstepsCollection CreateSearchSteps(const NfaWithVisitedChecks &nfa, const Declarations &declarations);
}


#endif //RBGGAMEMANAGER_SEARCHSTEP_CREATOR_H
