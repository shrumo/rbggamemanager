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
      search_steps_.emplace_back(std::move(step));
      return search_steps_.size() - 1;
    }

    Block* GetBlockPointer(uint index) {
      return search_steps_[index].get();
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

    Block* initial() {
      return search_steps_[initial_index_].get();
    }

    void set_initial(uint initial_block_index) {
      initial_index_ = initial_block_index;
    }

  private:
    std::vector<std::unique_ptr<Block>> search_steps_;
    std::unique_ptr<ResettableBitArrayStack> visited_info_stack_;
    std::unique_ptr<ResettableBitArrayStack> conditions_results_;
    uint visited_checks_count_;
    uint condition_results_count_;

    std::unordered_map<uint, const ModifyingApplication*> modifiers_;
    std::unordered_map<uint, Block*> switches_;
    uint initial_index_;
  };

  BlocksCollection CreateSearchSteps(const Nfa<std::unique_ptr<Move>> &nfa, const Declarations &declarations);
}


#endif //RBGGAMEMANAGER_BLOCKS_CREATOR_H
