//
// Created by shrum on 17.06.19.
//

#ifndef RBGGAMEMANAGER_ARITHMETIC_CREATOR_H
#define RBGGAMEMANAGER_ARITHMETIC_CREATOR_H

#include <game_description/moves/arithmetic_operation.h>
#include <parser/parser_wrapper.h>

namespace rbg {
std::unique_ptr<rbg::ArithmeticOperation> CreateArithmeticOperation(
    const rbg_parser::arithmetic_expression &expression,
    const Declarations &declarations);

std::unique_ptr<rbg::ArithmeticOperation> CreateArithmeticOperation(
    const ArithmeticOperation &arithmeticOperation);
}  // namespace rbg

#endif  // RBGGAMEMANAGER_ARITHMETIC_CREATOR_H
