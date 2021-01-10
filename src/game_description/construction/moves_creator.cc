//
// Created by shrum on 16.06.19.
//

#include "moves_creator.h"

#include <parser/parser_wrapper.h>

#include "arithmetic_creator.h"
#include "game_description/construction/graph_creator.h"

namespace {

using namespace rbg;
using namespace std;

ComparisonType comparison_type(rbg_parser::kind kind) {
  switch (kind) {
    case rbg_parser::eq:
      return ComparisonType::kEqual;
    case rbg_parser::neq:
      return ComparisonType::kNotEqual;
    case rbg_parser::le:
      return ComparisonType::kLess;
    case rbg_parser::leq:
      return ComparisonType::kLessEqual;
    case rbg_parser::ge:
      return ComparisonType::kGreater;
    case rbg_parser::geq:
      return ComparisonType::kGreaterEqual;
  }
  assert(false && "Not supported kind of comparison.");
}

class MovesCreator : public AstFunction<unique_ptr<Move>> {
 public:
  explicit MovesCreator(const Declarations &declarations)
      : declarations_(declarations) {}

  unique_ptr<Move> ShiftCase(const rbg_parser::shift &move) override {
    auto result =
        make_unique<Shift>(declarations_.initial_board().edges_names().Id(
                               move.get_content().to_string()),
                           move.index_in_expression());
    result->set_original_move(&move);
    return result;
  }

  unique_ptr<Move> OnCase(const rbg_parser::ons &move) override {
    std::vector<bool> pieces_table(declarations_.pieces_resolver().size());
    for (const auto &piece : move.get_legal_ons()) {
      pieces_table[declarations_.pieces_resolver().Id(piece.to_string())] =
          true;
    }
    auto result = make_unique<On>(pieces_table, move.index_in_expression());
    result->set_original_move(&move);
    return result;
  }

  unique_ptr<Move> OffCase(const rbg_parser::off &move) override {
    auto result = make_unique<Off>(
        declarations_.pieces_resolver().Id(move.get_piece().to_string()),
        move.index_in_expression());
    result->set_original_move(&move);
    return result;
  }

  unique_ptr<Move> AssignmentCase(const rbg_parser::assignment &move) override {
    auto result = make_unique<Assignment>(
        declarations_.variables_resolver().Id(move.get_left_side().to_string()),
        CreateArithmeticOperation(*move.get_right_side(), declarations_),
        move.index_in_expression());
    result->set_original_move(&move);
    return result;
  }

  unique_ptr<Move> PlayerSwitchCase(
      const rbg_parser::player_switch &move) override {
    auto result = make_unique<PlayerSwitch>(
        declarations_.players_resolver().Id(move.get_player().to_string()),
        move.index_in_expression());
    result->set_original_move(&move);
    return result;
  }

  unique_ptr<Move> KeeperSwitchCase(
      const rbg_parser::keeper_switch &move) override {
    auto result = make_unique<KeeperSwitch>(declarations_.keeper_id(),
                                            move.index_in_expression());
    result->set_original_move(&move);
    return result;
  }

  unique_ptr<Move> ArithmeticComparisonCase(
      const rbg_parser::arithmetic_comparison &move) override {
    auto result = make_unique<ArithmeticComparison>(
        CreateArithmeticOperation(*move.get_left_side(), declarations_),
        CreateArithmeticOperation(*move.get_right_side(), declarations_),
        comparison_type(move.get_kind_of_comparison()),
        move.index_in_expression());
    result->set_original_move(&move);
    return result;
  }

  unique_ptr<Move> NoopCase(const rbg_parser::noop &move) override {
    auto result = make_unique<Noop>();
    result->set_original_move(&move);
    return result;
  }

 private:
  const Declarations &declarations_;
};
}  // namespace

unique_ptr<Move> rbg::CreateMove(const rbg_parser::game_move &move,
                                 const Declarations &declarations) {
  return MovesCreator(declarations)(move);
}