//
// Created by shrum on 20.05.19.
//

#ifndef RBGGAMEMANAGER_PARSER_WRAPPER_H
#define RBGGAMEMANAGER_PARSER_WRAPPER_H


#include <rbgParser/src/abstract_dispatcher.hpp>


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

template<typename ResultType>
class AstVisitorR : public rbg_parser::abstract_dispatcher {
public:
  virtual ResultType visit(const rbg_parser::sum &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::prioritized_sum &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::concatenation &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::star_move &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::shift &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::ons &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::off &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::assignment &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::player_switch &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::keeper_switch &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::move_check &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::arithmetic_comparison &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::integer_arithmetic &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::variable_arithmetic &) { return ResultType(); }

  virtual ResultType visit(const rbg_parser::arithmetic_operation &) { return ResultType(); }

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
ResultType Fn(const NodeType &move, Args... args) {
  Visitor visitor(std::forward<Args>(args)...);
  move.accept(visitor);
  return std::move(visitor.Result());
}



#endif //RBGGAMEMANAGER_PARSER_WRAPPER_H
