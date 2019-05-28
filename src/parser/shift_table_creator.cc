//
// Created by shrum on 20.05.19.
//

#include "shift_table_creator.h"
#include <algorithm>

void ShiftTableCreator::dispatch(const rbg_parser::sum &sum) {
  unsigned int result_clear_length = 0;
  std::vector<std::unordered_set<vertex_t> > tmp_result = std::vector<std::unordered_set<vertex_t> >(board_.size());
  for (auto &item : sum.get_content()) {
    item->accept(*this);
    if (!clear_length_)
      return;
    result_clear_length = std::max(clear_length_ + 1, result_clear_length);
    auto rec_result = ExtractResult();
    for (vertex_t v = 0; v < static_cast<ssize_t >(rec_result.size()); v++) {
      tmp_result[v].insert(rec_result[v].begin(), rec_result[v].end());
    }
    ResetResult();
  }
  result_ = tmp_result;
  clear_length_ = result_clear_length;
}

void ShiftTableCreator::dispatch(const rbg_parser::concatenation &concat) {
  unsigned int result_clear_length = 0;
  std::vector<std::unordered_set<vertex_t> > result_table = std::vector<std::unordered_set<vertex_t> >(board_.size());

  std::vector<std::vector<std::unordered_set<vertex_t> > > concat_tables;
  std::vector<unsigned int> result_concat_clear_lengths;
  std::vector<unsigned int> result_concat_begins;

  for (ssize_t j = 0; j < static_cast<ssize_t >(result_table.size()); j++) {
    result_table[j] = {j};
  }
  for (size_t i = 0; i < concat.get_content().size(); i++) {
    auto &item = concat.get_content()[i];
    item->accept(*this);
    if (!clear_length_) {
      if (i == 0) {
        ResetResult();
        continue;
      }
      concat_tables.push_back(std::move(result_table));
      result_concat_clear_lengths.push_back(result_clear_length);
      result_concat_begins.push_back(static_cast<unsigned int &&>(i));
      result_clear_length = 0;
      result_table = std::vector<std::unordered_set<vertex_t> >(board_.size());
      for (ssize_t j = 0; j < static_cast<ssize_t >(result_table.size()); j++) {
        result_table[j] = {j};
      }
      ResetResult();
      continue;
    } else if (result_clear_length == 0 && clear_length_ && i != 0) {
      concat_tables.push_back(std::move(result_table));
      result_concat_clear_lengths.push_back(result_clear_length);
      result_concat_begins.push_back(static_cast<unsigned int &&>(i));
      result_clear_length = 0;
      result_table = std::vector<std::unordered_set<vertex_t> >(board_.size());
      for (ssize_t j = 0; j < static_cast<ssize_t >(result_table.size()); j++) {
        result_table[j] = {j};
      }
    }
    result_clear_length += clear_length_;
    auto tmp_rec_result = ExtractResult();
    std::vector<std::unordered_set<vertex_t> > next_result(result_table.size());
    for (vertex_t v = 0; v < static_cast<ssize_t >(result_table.size()); v++) {
      for (auto neighbour : result_table[v]) {
        if (neighbour != -1)
          next_result[v].insert(tmp_rec_result[neighbour].begin(), tmp_rec_result[neighbour].end());
      }
    }
    result_table = next_result;
    ResetResult();
  }
  concat_tables.push_back(result_table);
  result_concat_clear_lengths.push_back(result_clear_length);
  result_concat_begins.push_back(static_cast<unsigned int &&>(concat.get_content().size()));
  result_ = result_table;
  clear_length_ = *std::min_element(result_concat_clear_lengths.begin(), result_concat_clear_lengths.end());

  concat_results_ = concat_tables;
  concat_clear_lengths_ = result_concat_clear_lengths;
  concat_ends_ = result_concat_begins;
}

void ShiftTableCreator::dispatch(const rbg_parser::star_move &move) {
  unsigned int tmp_clear_length = 0;
  std::vector<std::unordered_set<vertex_t> > tmp_result = std::vector<std::unordered_set<vertex_t> >(board_.size());
  for (ssize_t j = 0; j < static_cast<ssize_t >(tmp_result.size()); j++) {
    tmp_result[j] = {j};
  }

  move.get_content()->accept(*this);
  if (!clear_length_)
    return;
  tmp_clear_length += clear_length_;
  auto tmp_rec_result = ExtractResult();

  for (uint i = 0; i < board_.size(); i++) {
    for (vertex_t v = 0; v < static_cast<ssize_t >(tmp_result.size()); v++) {
      for (auto neighbour : tmp_result[v]) {
        if (neighbour != -1)
          tmp_result[v].insert(tmp_rec_result[neighbour].begin(), tmp_rec_result[neighbour].end());
      }
    }
  }
  ResetResult();
  result_ = tmp_result;
  clear_length_ = tmp_clear_length;
}

void ShiftTableCreator::dispatch(const rbg_parser::shift &action) {
  ResetResult();
  for (vertex_t v = 0; v < static_cast<ssize_t >(result_.size()); v++) {
    result_[v] = {board_.Next(v, edge_resolver_.Id(action.get_content().to_string()))};
  }
  clear_length_ = 1;
}

void ShiftTableCreator::dispatch(const rbg_parser::move_check &move) {
  unsigned int tmp_clear_length = 0;
  std::vector<std::unordered_set<vertex_t> > tmp_result = std::vector<std::unordered_set<vertex_t> >(board_.size());
  for (ssize_t j = 0; j < static_cast<ssize_t >(tmp_result.size()); j++) {
    tmp_result[j] = {j};
  }

  move.get_content()->accept(*this);
  if (!clear_length_)
    return;
  tmp_clear_length = clear_length_;
  auto tmp_rec_result = ExtractResult();

  if (!move.is_negated()) {
    for (vertex_t v = 0; v < static_cast<ssize_t >(tmp_result.size()); v++) {
      bool exists = false;
      for (auto neighbour : tmp_rec_result[v]) {
        if (neighbour != -1) {
          exists = true;
          tmp_result[v] = {v};
          break;
        }
      }
      if (!exists) {
        tmp_result[v] = {-1};
      }
    }
  } else {
    for (vertex_t v = 0; v < static_cast<ssize_t >(tmp_result.size()); v++) {
      bool exists = false;
      for (auto neighbour : tmp_rec_result[v]) {
        if (neighbour != -1) {
          exists = true;
          tmp_result[v] = {-1};
          break;
        }
      }
      if (!exists) {
        tmp_result[v] = {v};
      }
    }
  }

  ResetResult();
  result_ = tmp_result;
  clear_length_ = tmp_clear_length;

}
