//
// Created by shrum on 07.05.18.
//

#ifndef RBGGAMEMANAGER_ARITHMETIC_CREATOR_H
#define RBGGAMEMANAGER_ARITHMETIC_CREATOR_H

#include"../../rbgParser/src/sum.hpp"
#include"../../rbgParser/src/concatenation.hpp"
#include"../../rbgParser/src/star_move.hpp"
#include"../../rbgParser/src/shift.hpp"
#include"../../rbgParser/src/ons.hpp"
#include"../../rbgParser/src/offs.hpp"
#include"../../rbgParser/src/assignments.hpp"
#include"../../rbgParser/src/switch.hpp"
#include"../../rbgParser/src/arithmetic_comparison.hpp"
#include"../../rbgParser/src/integer_arithmetic.hpp"
#include"../../rbgParser/src/variable_arithmetic.hpp"
#include"../../rbgParser/src/arithmetic_operation.hpp"
#include "../game_components/name_resolver.h"
#include "../actions/arithmetic_operation.h"

std::unique_ptr<ArithmeticOperation> CreateOperation(const rbg_parser::arithmetic_expression& expression, const NameResolver& resolver);




#endif //RBGGAMEMANAGER_ARITHMETIC_CREATOR_H
