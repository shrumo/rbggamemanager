//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_CONDITION_H
#define RBGGAMEMANAGER_CONDITION_H

#include <vector>
#include <memory>
#include "arithmetic_operation.h"
#include "../game_nfa/automaton.h"
#include <limits>

class game_state;
class action;

class condition {
public:
    virtual bool check(game_state *b)const =0;
};

namespace conditions
{
    class conjunction : public condition
    {
        std::vector<std::unique_ptr<condition> > items;
    public:
        conjunction(std::vector<std::unique_ptr<condition> > items) : items(std::move(items))
        {}
        bool check(game_state *b) const override
        {
            for(auto& cond : items)
            {
                if(!cond->check(b))
                    return false;
            }
            return true;
        }
    };

    class alternative : public condition
    {
        std::vector<std::unique_ptr<condition> > items;
    public:
        alternative(std::vector<std::unique_ptr<condition> > items) : items(std::move(items))
        {}
        bool check(game_state *b) const override
        {
            for(auto& cond : items)
            {
                if(cond->check(b))
                    return true;
            }
            return false;
        }
    };

    class negation : public condition
    {
        std::unique_ptr<condition> item;
    public:
        negation(std::unique_ptr<condition> item) : item(std::move(item))
        {}
        bool check(game_state *b) const override
        {
            return !item->check(b);
        }
    };

    class less : public condition
    {
        std::unique_ptr<arithmetic_operation> left, right;
    public:
        less(std::unique_ptr<arithmetic_operation> left, std::unique_ptr<arithmetic_operation> right)
            : left(std::move(left)), right(std::move(right))  {}
        bool check(game_state *b) const override
        {
            return left->value(b) < right->value(b);
        }
    };

    class less_equal : public condition
    {
        std::unique_ptr<arithmetic_operation> left, right;
    public:
        less_equal(std::unique_ptr<arithmetic_operation> left, std::unique_ptr<arithmetic_operation> right)
                : left(std::move(left)), right(std::move(right))  {}
        bool check(game_state *b) const override
        {
            return left->value(b) <= right->value(b);
        }
    };

    class equal : public condition
    {
        std::unique_ptr<arithmetic_operation> left, right;
    public:
        equal(std::unique_ptr<arithmetic_operation> left, std::unique_ptr<arithmetic_operation> right)
        : left(std::move(left)), right(std::move(right))  {}
        bool check(game_state *b) const override
        {
            return left->value(b) == right->value(b);
        }
    };

    class not_equal : public condition
    {
        std::unique_ptr<arithmetic_operation> left, right;
    public:
        not_equal(std::unique_ptr<arithmetic_operation> left, std::unique_ptr<arithmetic_operation> right)
        : left(std::move(left)), right(std::move(right))  {}
        bool check(game_state *b) const override
        {
            return left->value(b) != right->value(b);
        }
    };

    class move_pattern : public condition
    {
        unsigned int index;
        std::unique_ptr<fsm::nfa<action*> > move_nfa;
    public:
        move_pattern(unsigned int index, std::unique_ptr<fsm::nfa<action*> > move_nfa)
                : index(index), move_nfa(std::move(move_nfa))
        {}
        bool check(game_state *b) const override;
    };
}


#endif //RBGGAMEMANAGER_CONDITION_H
