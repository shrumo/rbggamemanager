//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_ARITHMETIC_OPERATION_H
#define RBGGAMEMANAGER_ARITHMETIC_OPERATION_H

#include "../game_components/name_resolver.h"

class game_state;

class arithmetic_operation {
public:
    virtual int value(game_state *b) const=0;
};

namespace arithmetic_operations
{
    class variable_value : public arithmetic_operation
    {
        token_id_t variable;
    public:
        variable_value(token_id_t variable) : variable(variable) {}
        int value(game_state *b) const override;
    };

    class constant_value : public arithmetic_operation
    {
        int constant;
    public:
        constant_value(int constant) : constant(constant) {}
        int value(game_state *b) const override { return constant; };
    };
}


#endif //RBGGAMEMANAGER_ARITHMETIC_OPERATION_H
