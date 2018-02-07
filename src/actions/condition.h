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

enum class condition_type : char
{
    CONJUNCTION_T,
    ALTERNATIVE_T,
    NEGATION_T,
    LESS_T,
    LESS_EQUAL_T,
    EQUAL_T,
    NOT_EQUAL_T,
    MOVE_PATTERN_T,
};

class condition {
    condition_type type;
protected:
    explicit condition(condition_type type) : type(type) {};
public:
    virtual bool check(game_state *b)const =0;
    virtual ~condition()= default;
    condition_type get_type() const { return type; }
};

namespace conditions
{
    class conjunction : public condition
    {
        std::vector<std::unique_ptr<condition> > items;
    public:
        conjunction(std::vector<std::unique_ptr<condition> > items) : condition(condition_type::CONJUNCTION_T),
                                                                      items(std::move(items))
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

        const std::vector<std::unique_ptr<condition>>& get_items() const
        {
            return items;
        }
    };

    class alternative : public condition
    {
        std::vector<std::unique_ptr<condition> > items;
    public:
        alternative(std::vector<std::unique_ptr<condition> > items) : condition(condition_type ::ALTERNATIVE_T),
                                                                      items(std::move(items))
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

        const std::vector<std::unique_ptr<condition>>& get_items() const
        {
            return items;
        }
    };

    class negation : public condition
    {
        std::unique_ptr<condition> item;
    public:
        negation(std::unique_ptr<condition> item) : condition(condition_type::NEGATION_T),
                                                    item(std::move(item))
        {}
        bool check(game_state *b) const override
        {
            return !item->check(b);
        }

        const condition* get_condition() const
        {
            return item.get();
        }
    };

    class less : public condition
    {
        std::unique_ptr<arithmetic_operation> left, right;
    public:
        less(std::unique_ptr<arithmetic_operation> left, std::unique_ptr<arithmetic_operation> right)
            : condition(condition_type::LESS_T), left(std::move(left)), right(std::move(right))  {}
        bool check(game_state *b) const override
        {
            return left->value(b) < right->value(b);
        }
        const arithmetic_operation* get_left() const
        {
            return left.get();
        }

        const arithmetic_operation* get_right() const
        {
            return right.get();
        }
    };

    class less_equal : public condition
    {
        std::unique_ptr<arithmetic_operation> left, right;
    public:
        less_equal(std::unique_ptr<arithmetic_operation> left, std::unique_ptr<arithmetic_operation> right)
                : condition(condition_type :: LESS_EQUAL_T), left(std::move(left)), right(std::move(right))  {}
        bool check(game_state *b) const override
        {
            return left->value(b) <= right->value(b);
        }
        const arithmetic_operation* get_left() const
        {
            return left.get();
        }

        const arithmetic_operation* get_right() const
        {
            return right.get();
        }

    };

    class equal : public condition
    {
        std::unique_ptr<arithmetic_operation> left, right;
    public:
        equal(std::unique_ptr<arithmetic_operation> left, std::unique_ptr<arithmetic_operation> right)
        : condition(condition_type::EQUAL_T), left(std::move(left)), right(std::move(right))  {}
        bool check(game_state *b) const override
        {
            return left->value(b) == right->value(b);
        }
        const arithmetic_operation* get_left() const
        {
            return left.get();
        }

        const arithmetic_operation* get_right() const
        {
            return right.get();
        }
    };

    class not_equal : public condition
    {
        std::unique_ptr<arithmetic_operation> left, right;
    public:
        not_equal(std::unique_ptr<arithmetic_operation> left, std::unique_ptr<arithmetic_operation> right)
        : condition(condition_type::NOT_EQUAL_T), left(std::move(left)), right(std::move(right))  {}
        bool check(game_state *b) const override
        {
            return left->value(b) != right->value(b);
        }
        const arithmetic_operation* get_left() const
        {
            return left.get();
        }

        const arithmetic_operation* get_right() const
        {
            return right.get();
        }
    };

    class move_pattern : public condition
    {
        unsigned int index;
        std::unique_ptr<fsm::nfa<const action*> > move_nfa;
    public:
        move_pattern(unsigned int index, std::unique_ptr<fsm::nfa<const action*> > move_nfa)
                : condition(condition_type ::MOVE_PATTERN_T), index(index), move_nfa(std::move(move_nfa))
        {}
        bool check(game_state *b) const override;

        const fsm::nfa<const action*>* get_nfa() const
        {
            return move_nfa.get();
        }
    };
}


#endif //RBGGAMEMANAGER_CONDITION_H
