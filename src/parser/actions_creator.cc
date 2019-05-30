//
// Created by shrum on 28.05.19.
//

#include "actions_creator.h"
#include "parser_wrapper.h"
#include "parser_actions.h"
#include "arithmetic_operations_creator.h"
#include "graph_creator.h"
#include <actions/actions.h>
#include <vector>
#include <string>

using namespace rbg;

class ActionsCreator : public AstVisitorR<std::unique_ptr<Action>> {
public:
  explicit ActionsCreator(uint id) : id_(id) {}

  std::unique_ptr<Action> visit(const rbg_parser::shift &shift) override {
    return std::make_unique<Shift>(shift.get_content().to_string(), id_);
  }

  std::unique_ptr<Action> visit(const rbg_parser::ons &ons) override {
    std::vector<std::string> pieces;
    pieces.reserve(ons.get_legal_ons().size());
    for (auto &token : ons.get_legal_ons()) {
      pieces.push_back(token.to_string());
    }
    return std::make_unique<On>(pieces, id_);
  }

  std::unique_ptr<Action> visit(const rbg_parser::move_check &move) override {
    Nfa<std::unique_ptr<Action>> nfa_graph = CreateGraph(*move.get_content());
    return std::make_unique<ConditionCheck>(std::move(nfa_graph.graph), nfa_graph.initial, nfa_graph.final,
                                            move.is_negated() ? ActionType::kNegatedConditionCheckType
                                                              : ActionType::kConditionCheckType, id_);
  }

  std::unique_ptr<Action> visit(const rbg_parser::arithmetic_comparison &move) override {
    auto left = CreateArithmeticOperation(*move.get_left_side());
    auto right = CreateArithmeticOperation(*move.get_right_side());
    switch (move.get_kind_of_comparison()) {
      case rbg_parser::eq:
        return std::make_unique<ArithmeticComparison>(std::move(left), std::move(right),
                                                      ActionType::kArithmeticEqualComparisonType, id_);
      case rbg_parser::neq:
        return std::make_unique<ArithmeticComparison>(std::move(left), std::move(right),
                                                      ActionType::kArithmeticNotEqualComparisonType, id_);
      case rbg_parser::le:
        return std::make_unique<ArithmeticComparison>(std::move(left), std::move(right),
                                                      ActionType::kArithmeticLessComparisonType, id_);
      case rbg_parser::leq:
        return std::make_unique<ArithmeticComparison>(std::move(left), std::move(right),
                                                      ActionType::kArithmeticLessEqualComparisonType, id_);
      case rbg_parser::ge:
        return std::make_unique<ArithmeticComparison>(std::move(right), std::move(left),
                                                      ActionType::kArithmeticLessComparisonType, id_);
      case rbg_parser::geq:
        return std::make_unique<ArithmeticComparison>(std::move(right), std::move(left),
                                                      ActionType::kArithmeticLessEqualComparisonType, id_);
    }
    throw std::logic_error("Not implemented.");
  }

  std::unique_ptr<Action> visit(const rbg_parser::off &move) override {
    return std::make_unique<Off>(move.get_piece().to_string(), id_);
  }

  std::unique_ptr<Action> visit(const rbg_parser::assignment &move) override {
    return std::make_unique<Assignment>(move.get_left_side().to_string(),
                                        CreateArithmeticOperation(*move.get_right_side()), id_);
  }

  std::unique_ptr<Action> visit(const rbg_parser::player_switch &move) override {
    return std::make_unique<PlayerSwitch>(move.get_player().to_string(), id_);
  }

  std::unique_ptr<Action> visit(const rbg_parser::keeper_switch &) override {
    return std::make_unique<PlayerSwitch>(">", id_);
  }

  std::unique_ptr<Action> defaultCase() override {
    throw std::logic_error("This is not an action.");
  }

private:
  uint id_;
};

std::unique_ptr<rbg::Action> CreateAction(const rbg_parser::game_move &move, uint id) {
  return Fn<ActionsCreator, std::unique_ptr<rbg::Action> >(move, id);
}
