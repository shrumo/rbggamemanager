//
// Created by shrum on 14.01.18.
//

#include "search_context.h"
#include "game_state.h"
#include <algorithm>


void SearchContext::FindAllMovesRec(size_t visited_array_index,
                                    const fsm::Nfa<const Action *> &nfa,
                                    fsm::state_id_t current_state, Move *move,
                                    bool block_started) {
  size_t index = VisitedIndex(nfa, current_state);
  size_t depth = move->blocks().size();
  if (visited_[visited_array_index][depth][index])
    return;
  visited_[visited_array_index][depth].set(index);
  if ((ssize_t) depth == max_depth_)
    return;
  for (const auto &transition : nfa[current_state].transitions()) {
    if(transition.letter()->type() == ActionType::kShiftTableType)
    {
      auto shift_table = dynamic_cast<const actions::ShiftTable*>(transition.letter());
      auto previous_pos = calculation_state_->pos();
      for(auto next_pos : shift_table->table()[previous_pos])
      {
        if(next_pos != -1) {
          calculation_state_->SetPos(next_pos);
          FindAllMovesRec(visited_array_index, nfa, transition.target(), move);
        }
      }
      calculation_state_->SetPos(previous_pos);
      continue;
    }
    ActionResult result = transition.letter()->Apply(calculation_state_);
    if (result) {
      if (transition.letter()->IsModifier()) {
        if (!block_started) {
          move->AddBlock(calculation_state_->pos(),
                         transition.letter()->index());
          CreateVisitedLayers(visited_array_index, depth + 1);
          InvalidateResults();
        }
        if (transition.letter()->IsSwitch())
          possible_moves_.push_back(*move);
        else {
          FindAllMovesRec(visited_array_index, nfa, transition.target(), move,
                          true);
        }
        if (!block_started) {
          move->PopBlock();
        }
      } else {
          FindAllMovesRec(visited_array_index, nfa, transition.target(), move);
      }
    }
    transition.letter()->Revert(calculation_state_, result);
  }
}

size_t
SearchContext::VisitedIndex(const fsm::Nfa<const Action *> &nfa,
                            fsm::state_id_t current_state) const {
  return (calculation_state_->pos()) * nfa.StateCount() + current_state;
}

size_t SearchContext::NewVisited(const fsm::Nfa<const Action *> &nfa) {
  if (last_visited_array_index_ < visited_.size()) {
    visited_[last_visited_array_index_].front().resize(
        std::max(visited_[last_visited_array_index_].front().size(),
                 calculation_state_->board().size() *
                 nfa.StateCount()));
    visited_[last_visited_array_index_].front().reset();
    return last_visited_array_index_++;
  }
  visited_.emplace_back();
  visited_.back().emplace_back(
      calculation_state_->board().size() *
      nfa.StateCount(), false);
  last_visited_array_index_ = visited_.size();
  return visited_.size() - 1;
}

size_t SearchContext::NewResultsCache(const fsm::Nfa<const Action *> &nfa) {
  if (last_results_array_index_ < results_.size()) {
    results_[last_results_array_index_].front().resize(
        std::max(results_[last_results_array_index_].front().size(),
                 calculation_state_->board().size() *
                 nfa.StateCount()));
    results_[last_results_array_index_].front().reset();
    return last_results_array_index_++;
  }
  results_.emplace_back();
  results_.back().emplace_back(
      calculation_state_->board().size() *
      nfa.StateCount(), false);
  last_results_array_index_ = results_.size();
  return results_.size() - 1;
}

bool SearchContext::CheckPlay(size_t visited_array_index, size_t results_index,
                              const fsm::Nfa<const Action *> &nfa,
                              fsm::state_id_t current_state, size_t depth,
                              bool block_started) {
  size_t index = VisitedIndex(nfa, current_state);
  if (current_state == nfa.final() || results_[results_index][depth][index]) {
    results_[results_index][depth].set(index);
    return true;
  }
  if (visited_[visited_array_index][depth][index])
    return false;
  visited_[visited_array_index][depth].set(index);
  if ((ssize_t) depth == max_depth_)
    return false;
  for (const auto &transition : nfa[current_state].transitions()) {
    if(transition.letter()->type() == ActionType::kShiftTableType)
    {
      auto shift_table = dynamic_cast<const actions::ShiftTable*>(transition.letter());
      auto previous_pos = calculation_state_->pos();
      for(auto next_pos : shift_table->table()[previous_pos])
      {
        if(next_pos != -1) {
          calculation_state_->SetPos(next_pos);
          if (CheckPlay(visited_array_index, results_index, nfa,
                        transition.target(), depth)) {
            results_[results_index][depth].set(index);
            calculation_state_->SetPos(previous_pos);
            return true;
          }
        }
        calculation_state_->SetPos(previous_pos);
      }
      continue;
    }
    ActionResult result = transition.letter()->Apply(calculation_state_);
    if (result) {
      if (transition.letter()->IsModifier()) {
        size_t new_depth = depth;
        if (!block_started) {
          CreateVisitedLayers(visited_array_index, depth + 1);
          CreateResultLayers(results_index, depth + 1);
          InvalidateResults(results_index);
          new_depth = depth + 1;
        }
        if (CheckPlay(visited_array_index, results_index, nfa,
                      transition.target(), new_depth, true) || transition.letter()->IsSwitch())
          results_[results_index][depth].set(index);
      } else if (CheckPlay(visited_array_index, results_index, nfa,
                           transition.target(), depth))
        results_[results_index][depth].set(index);
    }
    transition.letter()->Revert(calculation_state_, result);
    if (results_[results_index][depth][index])
      return true;
  }
  return false;
}

std::vector<Move>
SearchContext::FindMoves(GameState *state, ssize_t maximal_depth) {
  calculation_state_ = state;
  if (maximal_depth >= 0)
    max_depth_ = (size_t) maximal_depth;
  size_t visited_index = NewVisited(
      state->description().moves_description().nfa());
  Move empty;
  FindAllMovesRec(visited_index, state->description().moves_description().nfa(),
                  state->nfa_state(), &empty);
  DumpVisited(visited_index);
  calculation_state_ = nullptr;
  return std::move(possible_moves_);
}

bool SearchContext::CheckPattern(GameState *state,
                                 const fsm::Nfa<const Action *> &nfa,
                                 unsigned int move_pattern_index,
                                 ssize_t maximal_depth) {
  calculation_state_ = state;
  bool result = CheckPattern(nfa, move_pattern_index, maximal_depth);
  calculation_state_ = nullptr;
  return result;
}

void SearchContext::CreateVisitedLayers(size_t visited_array_index,
                                        size_t layer_depth) {
  if (visited_[visited_array_index].size() > layer_depth) {
    visited_[visited_array_index][layer_depth].resize(
        visited_[visited_array_index].front().size());
    visited_[visited_array_index][layer_depth].reset();
    return;
  }
  while (visited_[visited_array_index].size() <= layer_depth) {
    visited_[visited_array_index].emplace_back(
        visited_[visited_array_index].front().size(), false);
  }
}

void SearchContext::CreateResultLayers(size_t results_array_index,
                                       size_t layer_depth) {
  if (results_[results_array_index].size() > layer_depth) {
    results_[results_array_index][layer_depth].resize(
        results_[results_array_index].front().size());
    results_[results_array_index][layer_depth].reset();
    return;
  }
  while (results_[results_array_index].size() <= layer_depth) {
    results_[results_array_index].emplace_back(
        results_[results_array_index].front().size(), false);
  }
}

void SearchContext::DumpVisited(std::size_t visited_array_index) {
  if (visited_array_index == last_visited_array_index_ - 1) {
    last_visited_array_index_--;
  }
}

std::vector<Move>
SearchContext::FindFirstMove(GameState *state, ssize_t maximal_depth) {
  calculation_state_ = state;
  if (maximal_depth >= 0)
    max_depth_ = (size_t) maximal_depth;
  size_t visited_index = NewVisited(
      state->description().moves_description().nfa());
  Move empty;
  FindFirstMoveRec(visited_index,
                   state->description().moves_description().nfa(),
                   state->nfa_state(), &empty);
  DumpVisited(visited_index);
  calculation_state_ = nullptr;
  return std::move(possible_moves_);
}

bool SearchContext::FindFirstMoveRec(std::size_t visited_array_index,
                                     const fsm::Nfa<const Action *> &nfa,
                                     fsm::state_id_t current_state, Move *move,
                                     bool block_started) {
  size_t index = VisitedIndex(nfa, current_state);
  size_t depth = move->blocks().size();
  if (visited_[visited_array_index][depth][index])
    return false;
  visited_[visited_array_index][depth].set(index);
  if ((ssize_t) depth == max_depth_)
    return false;
  bool rec_result = false;
  for (const auto &transition : nfa[current_state].transitions()) {
    if(transition.letter()->type() == ActionType::kShiftTableType)
    {
      auto shift_table = dynamic_cast<const actions::ShiftTable*>(transition.letter());
      auto previous_pos = calculation_state_->pos();
      for(auto next_pos : shift_table->table()[previous_pos])
      {
        if(next_pos != -1) {
          calculation_state_->SetPos(next_pos);
          rec_result = FindFirstMoveRec(visited_array_index, nfa, transition.target(), move);
          if (rec_result) {
            calculation_state_->SetPos(previous_pos);
            return true;
          }
        }
      }
      calculation_state_->SetPos(previous_pos);
      continue;
    }
    ActionResult result = transition.letter()->Apply(calculation_state_);
    if (result) {
      if (transition.letter()->IsModifier()) {
        if (!block_started) {
          move->AddBlock(calculation_state_->pos(),
                         transition.letter()->index());
          CreateVisitedLayers(visited_array_index, depth + 1);
          InvalidateResults();
        }
        if (transition.letter()->IsSwitch()) {
          possible_moves_.push_back(*move);
          rec_result = true;
        } else {
          rec_result = FindFirstMoveRec(visited_array_index, nfa,
                                        transition.target(), move, true);
        }
        if (!block_started) {
          move->PopBlock();
        }
      } else
        rec_result = FindFirstMoveRec(visited_array_index, nfa,
                                      transition.target(), move);
    }
    transition.letter()->Revert(calculation_state_, result);
    if (rec_result)
      return true;
  }
  return false;
}

bool SearchContext::CheckPattern(const fsm::Nfa<const Action *> &nfa,
                                 unsigned int move_pattern_index,
                                 ssize_t maximal_depth) {
  if (maximal_depth >= 0)
    max_depth_ = (size_t) maximal_depth;
  size_t visited_index = NewVisited(nfa);
  size_t results_index;
  if (move_pattern_results_.find(move_pattern_index) !=
      move_pattern_results_.end())
    results_index = move_pattern_results_[move_pattern_index];
  else {
    results_index = NewResultsCache(nfa);
    move_pattern_results_[move_pattern_index] = results_index;
  }
  bool result = CheckPlay(visited_index, results_index, nfa, nfa.initial(), 0);
  DumpVisited(visited_index);
  return result;
}

