//
// Created by shrum on 14.01.18.
//

#ifndef RBGGAMEMANAGER_MOVES_INFORMATION_H
#define RBGGAMEMANAGER_MOVES_INFORMATION_H

#include <vector>
#include "../game_nfa/automaton.h"
#include "../utils/fast_bit_vector.h"
#include <unordered_map>
#include "move.h"

class GameState;

class Action;
namespace actions {
  class ConditionCheck;
  class NegatedConditionCheck;
}

struct PerftResult {
  size_t leaf_count;
  size_t node_count;
};

class SearchContext {
public:
  friend class actions::ConditionCheck;
  friend class actions::NegatedConditionCheck;

  SearchContext()
      :
      max_depth_(-1),
      last_visited_array_index_(0),
      last_results_array_index_(0),
      calculation_state_(nullptr) {
  }

  bool IsUsed() {
    return calculation_state_ != nullptr;
  }

  void clear() {
    last_visited_array_index_ = 0;
    last_results_array_index_ = 0;
    move_pattern_results_.clear();
  }

  void InvalidateResults(ssize_t omit = -1) {
    for (size_t i = 0; i < results_.size(); i++) {
      if ((ssize_t) i != omit) {
        for (auto &depth_results : results_[i]) {
          depth_results.reset();
        }
      }
    }
  }

  std::vector<Move> FindMoves(GameState *state, ssize_t max_depth = -1);

  PerftResult FindMovesDeep(GameState *state, size_t perft_depth, ssize_t max_depth=-1);

  std::vector<Move> FindFirstMove(GameState *state, ssize_t max_depth = -1);

  bool CheckPattern(GameState *state, const fsm::Nfa<const Action *> &nfa,
                    unsigned int move_pattern_index,
                    ssize_t max_depth = -1);

private:
  bool CheckPattern(const fsm::Nfa<const Action *> &nfa,
                    unsigned int move_pattern_index, ssize_t max_depth = -1);

  std::size_t NewVisited(const fsm::Nfa<const Action *> &nfa);

  std::size_t NewResultsCache(const fsm::Nfa<const Action *> &nfa);

  void DumpVisited(std::size_t visited_array_index);

  void
  CreateVisitedLayers(std::size_t visited_array_index, std::size_t layer_depth);

  void
  CreateResultLayers(std::size_t results_array_index, std::size_t layer_depth);

  PerftResult FastPerft(std::size_t visited_array_index,
                   const fsm::Nfa<const Action *> &nfa,
                   fsm::state_id_t current_state, size_t depth,
                   bool block_started = false, size_t depth_result = 3);

  void FindAllMovesRec(std::size_t visited_array_index,
                       const fsm::Nfa<const Action *> &nfa,
                       fsm::state_id_t current_state, Move *move,
                       bool block_started = false);

  bool FindFirstMoveRec(std::size_t visited_array_index,
                        const fsm::Nfa<const Action *> &nfa,
                        fsm::state_id_t current_state, Move *move,
                        bool block_started = false);

  bool CheckPlay(std::size_t visited_array_index, std::size_t results_index,
                 const fsm::Nfa<const Action *> &nfa,
                 fsm::state_id_t current_state, std::size_t depth,
                 bool block_started = false);

  std::size_t VisitedIndex(const fsm::Nfa<const Action *> &nfa,
                           fsm::state_id_t current_state) const;

  std::vector<Move> possible_moves_;
  ssize_t max_depth_;

  std::vector<std::vector<FastBitVector> > visited_;
  std::vector<std::vector<FastBitVector> > results_;

  std::size_t last_visited_array_index_;
  std::size_t last_results_array_index_;

  std::unordered_map<unsigned int, std::size_t> move_pattern_results_;

  GameState *calculation_state_;

};


#endif //RBGGAMEMANAGER_MOVES_INFORMATION_H
