//
// Created by shrum on 20.05.19.
//

#include "parser_utils.h"
#include "parser_wrapper.h"

class IsModifierVisitor : public AstVisitor {
public:
  void dispatch(const rbg_parser::sum &) override { result_ = false; }

  void dispatch(const rbg_parser::prioritized_sum &) override { result_ = false; }

  void dispatch(const rbg_parser::concatenation &) override { result_ = false; }

  void dispatch(const rbg_parser::star_move &) override { result_ = false; }

  void dispatch(const rbg_parser::shift &) override { result_ = false; }

  void dispatch(const rbg_parser::ons &) override { result_ = false; }

  void dispatch(const rbg_parser::off &) override { result_ = true; }

  void dispatch(const rbg_parser::assignment &) override { result_ = true; }

  void dispatch(const rbg_parser::player_switch &) override { result_ = true; }

  void dispatch(const rbg_parser::keeper_switch &) override { result_ = true; }

  void dispatch(const rbg_parser::move_check &) override { result_ = false; }

  void dispatch(const rbg_parser::arithmetic_comparison &) override { result_ = false; }

  void dispatch(const rbg_parser::integer_arithmetic &) override { result_ = false; }

  void dispatch(const rbg_parser::variable_arithmetic &) override { result_ = false; }

  void dispatch(const rbg_parser::arithmetic_operation &) override { result_ = false; }

  bool result() const { return result_; }

private:
  bool result_ = false;
};

bool IsModifier(const rbg_parser::game_move &m) {
  IsModifierVisitor fn;
  m.accept(fn);
  return fn.result();
}
