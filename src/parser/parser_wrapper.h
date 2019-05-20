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


#endif //RBGGAMEMANAGER_PARSER_WRAPPER_H
