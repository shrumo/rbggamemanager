//
// Created by shrum on 23.06.19.
//

#ifndef RBGGAMEMANAGER_BLOCKS_CREATOR_H
#define RBGGAMEMANAGER_BLOCKS_CREATOR_H

#include "utility/moves_visitor.h"
#include "game_description/construction/graph_creator.h"
#include <unordered_map>
#include <game_state/blocks/block.h>

namespace rbg {

  class BlocksCollection {
  public:
    BlocksCollection(uint visited_checks_count, uint condition_checks_count, uint board_size)
        : visited_info_stack_(std::make_unique<ResettableBitArrayStack>(visited_checks_count * board_size)),
          conditions_results_(std::make_unique<ResettableBitArrayStack>(condition_checks_count * board_size)),
          visited_checks_count_(visited_checks_count),
          condition_results_count_(condition_checks_count) {}

    uint AddBlock(std::unique_ptr<Block> step) {
      searchsteps_.emplace_back(std::move(step));
      return searchsteps_.size() - 1;
    }

    Block* GetBlockPointer(uint index) {
      return searchsteps_[index].get();
    }

    ResetableBitArrayStackChunk GetVisitedArrayChunk(uint index) {
      return {visited_info_stack_.get(),
              index * visited_info_stack_->bit_array_size() / visited_checks_count_,
              visited_info_stack_->bit_array_size() / visited_checks_count_};
    }

    ResetableBitArrayStackChunk GetConditionsResultsArrayChunk(uint index) {
      return {conditions_results_.get(),
              index * conditions_results_->bit_array_size() / condition_results_count_,
              conditions_results_->bit_array_size() / condition_results_count_};
    }

    void ResetStacks() {
      visited_info_stack_->reset();
      conditions_results_->reset();
    }

    void PushStacks() {
      visited_info_stack_->Push();
      conditions_results_->Push();
    }

    void PopStacks() {
      visited_info_stack_->Pop();
      conditions_results_->Pop();
    }

    void RegisterModifier(uint modifier_index, const ModifyingApplication* application) {
      modifiers_[modifier_index] = application;
    }

    void RegisterSwitch(uint modifier_index, Block* block) {
      switches_[modifier_index] = block;
    }

    const ModifyingApplication *Modifier(uint modifier_index) {
      return modifiers_.at(modifier_index);
    }

    Block *Switch(uint modifier_index) {
      return switches_.at(modifier_index);
    }

  private:
    std::vector<std::unique_ptr<Block>> searchsteps_;
    std::unique_ptr<ResettableBitArrayStack> visited_info_stack_;
    std::unique_ptr<ResettableBitArrayStack> conditions_results_;
    uint visited_checks_count_;
    uint condition_results_count_;

    std::unordered_map<uint, const ModifyingApplication*> modifiers_;
    std::unordered_map<uint, Block*> switches_;
  };

  class SearchStepsPoint {
  public:
    explicit SearchStepsPoint(BlocksCollection &parent_collection, Block *initial = nullptr) :
        parent_(&parent_collection), current_(initial), initial_(initial) {}

    void SetInitial(uint index) {
      initial_ = parent_->GetBlockPointer(index);
    }

    Block *current() {
      return current_;
    }

    void set_current(Block *step) {
      current_ = step;
    }

    void SetParent(BlocksCollection *parent) {
      parent_ = parent;
    }

    void reset() {
      current_ = initial_;
    }
  private:
    BlocksCollection *parent_;
    Block *current_;
    Block *initial_;
  };

  struct SearchStepsInformation {
    BlocksCollection collection;
    SearchStepsPoint current_position;
  };

  SearchStepsInformation CreateSearchSteps(const Nfa<std::unique_ptr<Move>> &nfa, const Declarations &declarations);
}


#endif //RBGGAMEMANAGER_BLOCKS_CREATOR_H
