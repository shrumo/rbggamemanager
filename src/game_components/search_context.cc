//
// Created by shrum on 14.01.18.
//

#include "search_context.h"
#include "game_state.h"
#include <algorithm>

// TODO(shrum): Add blocks of modifiers.

void SearchContext::FindAllMovesRec(size_t visited_array_index,
                                    const fsm::Nfa<const Action *> &nfa,
                                    fsm::state_id_t current_state, Move *move,
                                    ssize_t last_block_started) {
  const size_t index = VisitedIndex(nfa, current_state);
  const size_t depth = move->blocks().size();
  if (visited_[visited_array_index][depth][index])
    return;
  visited_[visited_array_index][depth].set(index);
  //if ((ssize_t) depth == max_depth_)
  //  return;
  for (const auto &transition : nfa[current_state].transitions()) {
    if(transition.letter()->type() == ActionType::kShiftTableType)
    {
      const auto shift_table = static_cast<const actions::ShiftTable*>(transition.letter());
      const auto previous_pos = calculation_state_->pos();
      for(auto next_pos : shift_table->table()[previous_pos])
      {
        calculation_state_->SetPos(next_pos);
        FindAllMovesRec(visited_array_index, nfa, transition.target(), move,last_block_started);
      }
      calculation_state_->SetPos(previous_pos);
      continue;
    }
    if (transition.letter()->IsSwitch()) {
        if (last_block_started != transition.letter()->index()) {
          move->AddBlock(calculation_state_->pos(),transition.letter()->index());
          possible_moves_.push_back(*move);
          move->PopBlock();
        } else possible_moves_.push_back(*move);
        continue;
    }
    ActionResult result = transition.letter()->Apply(calculation_state_);
    if (result) {
      if (transition.letter()->IsModifier()) {
        if (last_block_started != transition.letter()->index()) {
          move->AddBlock(calculation_state_->pos(),
                         transition.letter()->index());
          CreateVisitedLayers(visited_array_index, depth + 1);
        }
        FindAllMovesRec(visited_array_index, nfa, transition.target(), move,
                        transition.letter()->index());
        if (last_block_started != transition.letter()->index()) {
          move->PopBlock();
        }
      } else {
          FindAllMovesRec(visited_array_index, nfa, transition.target(), move,last_block_started);
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

bool SearchContext::CheckPlay(size_t visited_array_index,
                              const fsm::Nfa<const Action *> &nfa,
                              fsm::state_id_t current_state, size_t depth,
                              ssize_t last_block_started) {
  if (current_state == nfa.final()) {
    return true;
  }
  const size_t index = VisitedIndex(nfa, current_state);
  if (visited_[visited_array_index][depth][index])
    return false;
  visited_[visited_array_index][depth].set(index);
  //if ((ssize_t) depth == max_depth_)
  //  return false;
  for (const auto &transition : nfa[current_state].transitions()) {
    if(transition.letter()->type() == ActionType::kShiftTableType)
    {
      const auto shift_table = static_cast<const actions::ShiftTable*>(transition.letter());
      const auto previous_pos = calculation_state_->pos();
      for(auto next_pos : shift_table->table()[previous_pos])
      {
        calculation_state_->SetPos(next_pos);
        if (CheckPlay(visited_array_index, nfa,
                      transition.target(), depth,last_block_started)) {
          calculation_state_->SetPos(previous_pos);
          return true;
        }
      }
      calculation_state_->SetPos(previous_pos);
      continue;
    }
    ActionResult result = transition.letter()->Apply(calculation_state_);
    bool eval = false;
    if (result) {
      if (transition.letter()->IsModifier()) {
        size_t new_depth = depth;
        if (last_block_started != transition.letter()->index()) {
          CreateVisitedLayers(visited_array_index, depth + 1);
          new_depth = depth + 1;
        }
        if (CheckPlay(visited_array_index, nfa,
                      transition.target(), new_depth, transition.letter()->index()) || transition.letter()->IsSwitch())
          eval = true;
      } else if (CheckPlay(visited_array_index, nfa,
                           transition.target(), depth, last_block_started))
        eval = true;
    }
    transition.letter()->Revert(calculation_state_, result);
    if (eval)
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
                                     ssize_t last_block_started) {
  const size_t index = VisitedIndex(nfa, current_state);
  const size_t depth = move->blocks().size();
  if (visited_[visited_array_index][depth][index])
    return false;
  visited_[visited_array_index][depth].set(index);
  if ((ssize_t) depth == max_depth_)
    return false;
  bool rec_result = false;
  for (const auto &transition : nfa[current_state].transitions()) {
    if(transition.letter()->type() == ActionType::kShiftTableType)
    {
      auto shift_table = static_cast<const actions::ShiftTable*>(transition.letter());
      auto previous_pos = calculation_state_->pos();
      for(auto next_pos : shift_table->table()[previous_pos])
      {
        calculation_state_->SetPos(next_pos);
        rec_result = FindFirstMoveRec(visited_array_index, nfa, transition.target(), move, last_block_started);
        if (rec_result) {
          calculation_state_->SetPos(previous_pos);
          return true;
        }
      }
      calculation_state_->SetPos(previous_pos);
      continue;
    }
    if (transition.letter()->IsSwitch()) {
      if (last_block_started != transition.letter()->index()) {
        move->AddBlock(calculation_state_->pos(),
                       transition.letter()->index());
        possible_moves_.push_back(*move);
        move->PopBlock();
      } else possible_moves_.push_back(*move);
      return true;
    }
    ActionResult result = transition.letter()->Apply(calculation_state_);
    if (result) {
      if (transition.letter()->IsModifier()) {
        if (last_block_started != transition.letter()->index()) {
          move->AddBlock(calculation_state_->pos(),
                         transition.letter()->index());
          CreateVisitedLayers(visited_array_index, depth + 1);
        }
        rec_result = FindFirstMoveRec(visited_array_index, nfa,
                                      transition.target(), move, transition.letter()->index());
        if (last_block_started != transition.letter()->index()) {
          move->PopBlock();
        }
      } else
        rec_result = FindFirstMoveRec(visited_array_index, nfa,
                                      transition.target(), move, last_block_started);
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
  //if (maximal_depth >= 0)
  //  max_depth_ = (size_t) maximal_depth;
  const size_t visited_index = NewVisited(nfa);
  bool result = CheckPlay(visited_index, nfa, nfa.initial(), 0);
  DumpVisited(visited_index);
  return result;
}

PerftResult SearchContext::FastPerft(std::size_t visited_array_index, const fsm::Nfa<const Action *> &nfa,
                                fsm::state_id_t current_state, size_t depth, ssize_t last_block_started, size_t perft_depth) {
  const size_t index = VisitedIndex(nfa, current_state);
  if (visited_[visited_array_index][depth][index])
    return {0,0};
  visited_[visited_array_index][depth].set(index);
  if ((ssize_t) depth == max_depth_)
    return {0,0};
  size_t node_count = 0;
  size_t leaf_count = 0;
  for (const auto &transition : nfa[current_state].transitions()) {
    if(transition.letter()->type() == ActionType::kShiftTableType)
    {
      auto shift_table = static_cast<const actions::ShiftTable*>(transition.letter());
      auto previous_pos = calculation_state_->pos();
      for(auto next_pos : shift_table->table()[previous_pos])
      {
        calculation_state_->SetPos(next_pos);
        auto rec_result = FastPerft(visited_array_index, nfa, transition.target(), depth,last_block_started , perft_depth);
        node_count += rec_result.node_count;
        leaf_count += rec_result.leaf_count;
      }
      calculation_state_->SetPos(previous_pos);
      continue;
    }
    ActionResult result = transition.letter()->Apply(calculation_state_);
    if (result) {
      if (transition.letter()->IsModifier()) {
        size_t new_depth = depth;
        if (last_block_started != transition.letter()->index()) {
          new_depth = depth + 1;
          CreateVisitedLayers(visited_array_index, depth + 1);
        }
        if (transition.letter()->IsSwitch()) {
          node_count +=  static_cast<size_t>(result.revert_player() != calculation_state_->description().keeper_player_id());
          size_t next_perft = calculation_state_->player() == calculation_state_->description().keeper_player_id() ? perft_depth : perft_depth - 1;
          if (next_perft > 0) {
            auto rec_result = FastPerft(visited_array_index, nfa, transition.target(), new_depth, transition.letter()->index(), next_perft);
            node_count += rec_result.node_count;
            leaf_count += rec_result.leaf_count;
          }
          else {
            leaf_count += 1;
          }
        }
        else {
          auto rec_result = FastPerft(visited_array_index, nfa, transition.target(), new_depth, last_block_started, perft_depth);
          node_count += rec_result.node_count;
          leaf_count += rec_result.leaf_count;
        }
      } else {
        auto rec_result = FastPerft(visited_array_index, nfa, transition.target(), depth, last_block_started, perft_depth);
        node_count += rec_result.node_count;
        leaf_count += rec_result.leaf_count;
      }
    }
    transition.letter()->Revert(calculation_state_, result);
    if(calculation_state_->player() == calculation_state_->description().keeper_player_id() && node_count > 0)
    {
      return {leaf_count, node_count};
    }
  }
  return {leaf_count, node_count};
}

PerftResult SearchContext::FindMovesDeep(GameState *state, size_t perft_depth, ssize_t maximal_depth) {
  calculation_state_ = state;
  if (maximal_depth >= 0)
    max_depth_ = (size_t) maximal_depth;
  size_t visited_index = NewVisited(
      state->description().moves_description().nfa());
  Move empty;
  auto perft = FastPerft(visited_index, state->description().moves_description().nfa(),
                  state->nfa_state(), 0, false, perft_depth);
  DumpVisited(visited_index);
  calculation_state_ = nullptr;
  return {perft.leaf_count, perft.node_count};
}

