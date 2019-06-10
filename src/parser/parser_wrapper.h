#include <memory>

//
// Created by shrum on 20.05.19.
//

#ifndef RBGGAMEMANAGER_PARSER_VISITOR_H
#define RBGGAMEMANAGER_PARSER_VISITOR_H


#include <rbgParser/src/abstract_dispatcher.hpp>
#include "parser_actions.h"
#include <utility>
#include <stdexcept>
#include <rbgParser/src/parsed_game.hpp>
#include <rbgParser/src/game_items.hpp>
#include <string>
#include <cassert>

namespace rbg {
  std::unique_ptr<rbg_parser::parsed_game> ParseGame(const std::string &game_text);
  
  
// This is a visitor class for the abstract syntax tree that is returned by the parser.
// This allows to write functions that operate on the tree. It can be treated as a pattern matching function.
// The pattern is matched to the type of the game move on the input. Tha pattern is matched to the most specific type,
// so for example if gameMoveCase is overriden and sumCase is overriden, since sumCase is overriden, then even if
// sum is a game move, the sumCase will be chosen.
// The function can be called using the Fn<Function> syntax.
  template<typename ResultType>
  class AstFunction : public rbg_parser::abstract_dispatcher {
  public:
    using OutType = ResultType;

    virtual ResultType SumCase(const rbg_parser::sum &move) { return GameMoveCase(move); }

    virtual ResultType PrioritizedSumCase(const rbg_parser::prioritized_sum &move) { return GameMoveCase(move); }

    virtual ResultType ConcatenationCase(const rbg_parser::concatenation &move) { return GameMoveCase(move); }

    virtual ResultType StarCase(const rbg_parser::star_move &move) { return GameMoveCase(move); }

    virtual ResultType ShiftCase(const rbg_parser::shift &move) { return GameMoveCase(move); }

    virtual ResultType OnCase(const rbg_parser::ons &move) { return GameMoveCase(move); }

    virtual ResultType OffCase(const rbg_parser::off &move) { return GameMoveCase(move); }

    virtual ResultType AssignmentCase(const rbg_parser::assignment &move) { return GameMoveCase(move); }

    virtual ResultType PlayerSwitchCase(const rbg_parser::player_switch &move) { return GameMoveCase(move); }

    virtual ResultType KeeperSwitchCase(const rbg_parser::keeper_switch &move) { return GameMoveCase(move); }

    virtual ResultType MoveCheckCase(const rbg_parser::move_check &move) { return GameMoveCase(move); }

    virtual ResultType ArithmeticComparisonCase(const rbg_parser::arithmetic_comparison &move) {
      return GameMoveCase(move);
    }

    virtual ResultType IntegerArithmeticCase(const rbg_parser::integer_arithmetic &expression) {
      return ArithmeticExpressionCase(expression);
    }

    virtual ResultType VariableArithmeticCase(const rbg_parser::variable_arithmetic &expression) {
      return ArithmeticExpressionCase(expression);
    }

    virtual ResultType ArithmeticOperationCase(const rbg_parser::arithmetic_operation &expression) {
      return ArithmeticExpressionCase(expression);
    }

    virtual ResultType ArithmeticExpressionCase(const rbg_parser::arithmetic_expression &) { return DefaultCase(); }

    virtual ResultType GameMoveCase(const rbg_parser::game_move &) { return DefaultCase(); }

    virtual ResultType DefaultCase() { return ResultType(); }

    template<typename NodeType>
    ResultType operator()(const NodeType &move) {
      move.accept(*this);
      assert(result_exists_ &&
             "You cannot reuse the result. Extraction of the result can happen exactly once after calling visit.");
      result_exists_ = false;
      return std::move(result_);
    }

  private:
    void dispatch(const rbg_parser::sum &m) override {
      result_ = SumCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::prioritized_sum &m) override {
      result_ = PrioritizedSumCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::concatenation &m) override {
      result_ = ConcatenationCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::star_move &m) override {
      result_ = StarCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::shift &m) override {
      result_ = ShiftCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::ons &m) override {
      result_ = OnCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::off &m) override {
      result_ = OffCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::assignment &m) override {
      result_ = AssignmentCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::player_switch &m) override {
      result_ = PlayerSwitchCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::keeper_switch &m) override {
      result_ = KeeperSwitchCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::move_check &m) override {
      result_ = MoveCheckCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::arithmetic_comparison &m) override {
      result_ = ArithmeticComparisonCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::integer_arithmetic &m) override {
      result_ = IntegerArithmeticCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::variable_arithmetic &m) override {
      result_ = VariableArithmeticCase(m);
      result_exists_ = true;
    }

    void dispatch(const rbg_parser::arithmetic_operation &m) override {
      result_ = ArithmeticOperationCase(m);
      result_exists_ = true;
    }

    bool result_exists_ = false;
    ResultType result_;
  };
}

namespace std {
  template<>
  struct hash<rbg_parser::token> {
    std::size_t operator()(const rbg_parser::token &k) const {
      using std::size_t;
      using std::hash;
      using std::string;

      return hash<std::string>()(k.to_string());
    }
  };
}

#endif //RBGGAMEMANAGER_PARSER_VISITOR_H
