//
// Created by shrum on 20.05.19.
//

#ifndef RBGGAMEMANAGER_PARSER_WRAPPER_H
#define RBGGAMEMANAGER_PARSER_WRAPPER_H


#include <rbgParser/src/abstract_dispatcher.hpp>
#include "parser_actions.h"
#include <utility>
#include <stdexcept>

class AstVisitor : public rbg_parser::abstract_dispatcher {
public:
  void dispatch(const rbg_parser::sum &) override {}

  void dispatch(const rbg_parser::prioritized_sum &) override {}

  void dispatch(const rbg_parser::concatenation &) override {}

  void dispatch(const rbg_parser::star_move &) override {}

  void dispatch(const rbg_parser::shift &) override {}

  void dispatch(const rbg_parser::ons &) override {}

  void dispatch(const rbg_parser::off &) override {}

  void dispatch(const rbg_parser::assignment &) override {}

  void dispatch(const rbg_parser::player_switch &) override {}

  void dispatch(const rbg_parser::keeper_switch &) override {}

  void dispatch(const rbg_parser::move_check &) override {}

  void dispatch(const rbg_parser::arithmetic_comparison &) override {}

  void dispatch(const rbg_parser::integer_arithmetic &) override {}

  void dispatch(const rbg_parser::variable_arithmetic &) override {}

  void dispatch(const rbg_parser::arithmetic_operation &) override {}
};

// This allows to write simple visitors on the tree. It can be treated as a switch statement on type of the nodes in the
// tree.
// Example:
//  // Visitor that counts the number of nodes in the ast. Call by Fn<NodeCountVisitor,int>(move).
//  class NodeCountVisitor : public AstVisitorR<int> {
//  public:
//    template<typename PointerToVisitableType>
//    int ElementsNodeCountSum(const std::vector<PointerToVisitableType> &children)
//    {
//      int result = 0;
//      for(const auto& child : children) {
//        result += Fn<NodeCountVisitor, int>(*child);
//      }
//      return result;
//    }
//    int visit(const rbg_parser::sum &move) override {
//      return ElementsNodeCountSum(move.get_content()) + 1;
//    }
//    int visit(const rbg_parser::prioritized_sum &move) override {
//      return ElementsNodeCountSum(move.get_content()) + 1;
//    }
//    int visit(const rbg_parser::concatenation &move) override {
//      return ElementsNodeCountSum(move.get_content()) + 1;
//    }
//    int visit(const rbg_parser::star_move &move) override {
//      return Fn<NodeCountVisitor, int>(*move.get_content()) + 1;
//    }
//    int visit(const rbg_parser::move_check &move) override {
//      return Fn<NodeCountVisitor, int>(*move.get_content()) + 1;
//    }
//    int visit(const rbg_parser::arithmetic_comparison &move) override {
//      return
//          ElementsNodeCountSum<const rbg_parser::arithmetic_expression *>({move.get_left_side(), move.get_right_side()}) + 1;
//    }
//    int visit(const rbg_parser::arithmetic_operation &move) override {
//      return ElementsNodeCountSum(move.get_content()) + 1; }
//    int defaultCase() override {
//      return 1;
//    }
//  };
template<typename ResultType>
class AstVisitorR : public rbg_parser::abstract_dispatcher {
public:
  virtual ResultType defaultCase() { return ResultType(); }

  virtual ResultType gameMoveCase(const rbg_parser::game_move &) { return defaultCase(); }

  virtual ResultType arithmeticExpressionCase(const rbg_parser::arithmetic_expression &) { return defaultCase(); }

  virtual ResultType visit(const rbg_parser::sum &move) { return gameMoveCase(move); }

  virtual ResultType visit(const rbg_parser::prioritized_sum &move) { return gameMoveCase(move); }

  virtual ResultType visit(const rbg_parser::concatenation &move) { return gameMoveCase(move); }

  virtual ResultType visit(const rbg_parser::star_move &move) { return gameMoveCase(move); }

  virtual ResultType visit(const rbg_parser::shift &move) { return gameMoveCase(move); }

  virtual ResultType visit(const rbg_parser::ons &move) { return gameMoveCase(move); }

  virtual ResultType visit(const rbg_parser::off &move) { return gameMoveCase(move); }

  virtual ResultType visit(const rbg_parser::assignment &move) { return gameMoveCase(move); }

  virtual ResultType visit(const rbg_parser::player_switch &move) { return gameMoveCase(move); }

  virtual ResultType visit(const rbg_parser::keeper_switch &move) { return gameMoveCase(move); }

  virtual ResultType visit(const rbg_parser::move_check &move) { return gameMoveCase(move); }

  virtual ResultType visit(const rbg_parser::arithmetic_comparison &move) { return gameMoveCase(move); }

  virtual ResultType visit(const rbg_parser::integer_arithmetic &expression) {
    return arithmeticExpressionCase(expression);
  }

  virtual ResultType visit(const rbg_parser::variable_arithmetic &expression) {
    return arithmeticExpressionCase(expression);
  }

  virtual ResultType visit(const rbg_parser::arithmetic_operation &expression) {
    return arithmeticExpressionCase(expression);
  }

  ResultType Result() {
    if (!result_exists_)
      throw std::logic_error(
          "You cannot reuse the result. Extraction of the result can happen exactly once after calling visit.");
    result_exists_ = false;
    return std::move(result_);
  }

private:
  void dispatch(const rbg_parser::sum &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::prioritized_sum &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::concatenation &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::star_move &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::shift &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::ons &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::off &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::assignment &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::player_switch &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::keeper_switch &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::move_check &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::arithmetic_comparison &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::integer_arithmetic &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::variable_arithmetic &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  void dispatch(const rbg_parser::arithmetic_operation &m) override {
    result_ = visit(m);
    result_exists_ = true;
  }

  bool result_exists_ = false;
  ResultType result_;
};

template<typename Visitor, typename ResultType, typename NodeType, typename ...Args>
ResultType Fn(const NodeType &move, Args &&... args) {
  Visitor visitor(std::forward<Args>(args)...);
  move.accept(visitor);
  return std::move(visitor.Result());
}



#endif //RBGGAMEMANAGER_PARSER_WRAPPER_H
