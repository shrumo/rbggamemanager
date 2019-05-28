//
// Created by shrum on 20.05.19.
//

#include "parser_utils.h"
#include "parser_wrapper.h"

class IsModifierVisitor : public AstVisitorR<bool> {
public:

  bool visit(const rbg_parser::off &) override { return true; }

  bool visit(const rbg_parser::assignment &) override { return true; }

  bool visit(const rbg_parser::player_switch &) override { return true; }

  bool visit(const rbg_parser::keeper_switch &) override { return true; }

  bool defaultCase() override { return false; }
};

bool IsModifier(const rbg_parser::game_move &m) {
  return Fn<IsModifierVisitor, bool>(m);
}
