//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_GAME_AUTOMATON_H
#define RBGGAMEMANAGER_GAME_AUTOMATON_H

#include <memory>
#include "automaton.h"
#include <optimized_actions/optimized_action.h>

class GameMovesDescription {
public:
  const fsm::Nfa<const OptimizedAction *> &nfa() const {
    return *nfa_;
  }

  fsm::state_id_t CorrespondingState(size_t block_id) const {
    return blocks_states_[block_id];
  }

  unsigned int MovePatternCount() const {
    return move_pattern_count_;
  }

  friend class GameNfaCreator;

private:
  /* TODO(shrum): Create other way of creating GameMovesDescription. */
  GameMovesDescription(std::unique_ptr<fsm::Nfa<const OptimizedAction *>> nfa,
                       std::vector<fsm::state_id_t> blocks_states,
                       std::vector<std::unique_ptr<OptimizedAction>> actions,
                       unsigned int move_pattern_count)
      : nfa_(std::move(nfa)),
        blocks_states_(std::move(blocks_states)),
        actions_(std::move(actions)),
        move_pattern_count_(move_pattern_count) {}

  std::unique_ptr<fsm::Nfa<const OptimizedAction *>> nfa_;
  std::vector<fsm::state_id_t> blocks_states_;
  std::vector<std::unique_ptr<OptimizedAction> > actions_;
  unsigned int move_pattern_count_;
};


#endif //RBGGAMEMANAGER_GAME_AUTOMATON_H
