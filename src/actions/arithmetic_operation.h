//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_ARITHMETIC_OPERATION_H
#define RBGGAMEMANAGER_ARITHMETIC_OPERATION_H

#include "../game_components/name_resolver.h"

class game_state;

enum class arithmetic_operation_type : char
{
    VARIABLE_T,
    CONSTANT_T,
};

class arithmetic_operation {
protected:
    arithmetic_operation_type type;
    explicit arithmetic_operation(arithmetic_operation_type type) : type(type) {}
public:
    virtual int value(game_state *b) const=0;
    virtual ~arithmetic_operation()=default;
    arithmetic_operation_type get_type() const { return type; }
};

namespace arithmetic_operations
{
    class variable_value : public arithmetic_operation
    {
        token_id_t variable;
    public:
        variable_value(token_id_t variable) :
                arithmetic_operation(arithmetic_operation_type :: VARIABLE_T),
                variable(variable) {}
        int value(game_state *b) const override;
        token_id_t get_variables() const
        {
            return variable;
        }
    };

    class constant_value : public arithmetic_operation
    {
        int constant;
    public:
        constant_value(int constant) :
                arithmetic_operation(arithmetic_operation_type::CONSTANT_T),
                constant(constant) {}
        int value(game_state* b) const override;;
        int get_constant()
        {
            return constant;
        }
    };
}


#endif //RBGGAMEMANAGER_ARITHMETIC_OPERATION_H
