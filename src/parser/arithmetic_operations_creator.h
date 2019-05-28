//
// Created by shrum on 28.05.19.
//

#ifndef RBGGAMEMANAGER_ARITHMETIC_OPERATIONS_CREATOR_H
#define RBGGAMEMANAGER_ARITHMETIC_OPERATIONS_CREATOR_H

#include <rbgParser/src/arithmetic_expression.hpp>
#include <actions/arithmetic.h>

std::unique_ptr<rbg::ArithmeticOperation>
CreateArithmeticOperation(const rbg_parser::arithmetic_expression &expression);

#endif //RBGGAMEMANAGER_ARITHMETIC_OPERATIONS_CREATOR_H
