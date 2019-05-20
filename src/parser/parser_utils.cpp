//
// Created by shrum on 20.05.19.
//

#include "parser_utils.h"
#include "parser_wrapper.h"

class IsModifierVisitor : public AstVisitorR<bool> {
public:
  bool visit(const rbg_parser::sum &) override { return false; }

  bool visit(const rbg_parser::prioritized_sum &) override { return false; }

  bool visit(const rbg_parser::concatenation &) override { return false; }

  bool visit(const rbg_parser::star_move &) override { return false; }

  bool visit(const rbg_parser::shift &) override { return false; }

  bool visit(const rbg_parser::ons &) override { return false; }

  bool visit(const rbg_parser::off &) override { return true; }

  bool visit(const rbg_parser::assignment &) override { return true; }

  bool visit(const rbg_parser::player_switch &) override { return true; }

  bool visit(const rbg_parser::keeper_switch &) override { return true; }

  bool visit(const rbg_parser::move_check &) override { return false; }

  bool visit(const rbg_parser::arithmetic_comparison &) override { return false; }

  bool visit(const rbg_parser::integer_arithmetic &) override { return false; }

  bool visit(const rbg_parser::variable_arithmetic &) override { return false; }

  bool visit(const rbg_parser::arithmetic_operation &) override { return false; }
};

bool IsModifier(const rbg_parser::game_move &m) {
  return Fn<IsModifierVisitor, bool>(m);
}
