//
// Created by shrum on 20.05.19.
//

#ifndef RBGGAMEMANAGER_SHIFT_TABLE_CREATOR_H
#define RBGGAMEMANAGER_SHIFT_TABLE_CREATOR_H

#include <unordered_set>
#include "parser_wrapper.h"
#include "parser_actions.h"
#include <game_components/name_resolver.h>
#include <game_components/graph_board.h>

class ShiftTableCreator : public AstVisitor {
public:
  explicit ShiftTableCreator(const NameResolver &resolver, const GraphBoard &board, const EdgeResolver &edge_resolver)
      : resolver_(resolver), board_(board), edge_resolver_(edge_resolver),
        clear_length_(0), result_(board.size()) {
    ResetResult();
  }

  uint clear_length() const {
    return clear_length_;
  }

  void ResetResult() {
    for (ssize_t i = 0; i < static_cast<ssize_t >(result_.size()); i++) {
      result_[i] = {i};
    }
    clear_length_ = 0;
    concat_ends_ = {};
    concat_results_ = {};
    concat_clear_lengths_ = {};
  }

  std::vector<std::unordered_set<vertex_t> > ExtractResult() {
    auto tmp = std::move(result_);
    result_ = std::vector<std::unordered_set<vertex_t> >(tmp.size());
    ResetResult();
    return tmp;
  }

  const std::vector<std::vector<std::unordered_set<vertex_t> > > &concat_results() const {
    return concat_results_;
  }

  const std::vector<unsigned int> &concat_clear_lengths() const {
    return concat_clear_lengths_;
  }

  const std::vector<unsigned int> &concat_begins() const {
    return concat_ends_;
  }

private:
  void dispatch(const rbg_parser::prioritized_sum &) override {
    throw std::logic_error{"Function not yet implemented."};
  }

  void dispatch(const rbg_parser::sum &sum) override;

  void dispatch(const rbg_parser::concatenation &concat) override;

  void dispatch(const rbg_parser::star_move &move) override;

  void dispatch(const rbg_parser::shift &action) override;

  void dispatch(const rbg_parser::ons &) override { ResetResult(); }

  void dispatch(const rbg_parser::off &) override { ResetResult(); }

  void dispatch(const rbg_parser::assignment &) override { ResetResult(); }

  void dispatch(const rbg_parser::player_switch &) override { ResetResult(); }

  void dispatch(const rbg_parser::keeper_switch &) override { ResetResult(); }

  void dispatch(const rbg_parser::arithmetic_comparison &) override { ResetResult(); }

  void dispatch(const rbg_parser::move_check &move) override;

  void dispatch(const rbg_parser::integer_arithmetic &) override { ResetResult(); };

  void dispatch(const rbg_parser::variable_arithmetic &) override { ResetResult(); };

  void dispatch(const rbg_parser::arithmetic_operation &) override { ResetResult(); };

  const NameResolver &resolver_;
  const GraphBoard &board_;
  const EdgeResolver &edge_resolver_;

  unsigned int clear_length_;
  std::vector<std::unordered_set<vertex_t> > result_;

  std::vector<std::vector<std::unordered_set<vertex_t> > > concat_results_;
  std::vector<unsigned int> concat_clear_lengths_;
  std::vector<unsigned int> concat_ends_;
};


#endif //RBGGAMEMANAGER_SHIFT_TABLE_CREATOR_H
