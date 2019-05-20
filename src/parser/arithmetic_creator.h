//
// Created by shrum on 07.05.18.
//

#ifndef RBGGAMEMANAGER_ARITHMETIC_CREATOR_H
#define RBGGAMEMANAGER_ARITHMETIC_CREATOR_H

#include "parser_actions.h"
#include <game_components/name_resolver.h>
#include <actions/arithmetic_operation.h>

std::unique_ptr<ArithmeticOperation> CreateOperation(const rbg_parser::arithmetic_expression& expression, const NameResolver& resolver);

#endif //RBGGAMEMANAGER_ARITHMETIC_CREATOR_H
