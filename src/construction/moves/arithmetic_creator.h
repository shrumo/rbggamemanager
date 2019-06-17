//
// Created by shrum on 17.06.19.
//

#ifndef RBGGAMEMANAGER_ARITHMETIC_CREATOR_H
#define RBGGAMEMANAGER_ARITHMETIC_CREATOR_H

#include <parser/parser_wrapper.h>
#include <gamestate/searchsteps/arithmetic_operation.h>

namespace rbg {
  std::unique_ptr<rbg::ArithmeticOperation>
  CreateArithmeticOperation(const rbg_parser::arithmetic_expression &expression, const Declarations &declarations);
}

#endif //RBGGAMEMANAGER_ARITHMETIC_CREATOR_H
