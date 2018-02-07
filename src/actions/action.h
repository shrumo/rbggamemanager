//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_ACTION_H
#define RBGGAMEMANAGER_ACTION_H

#include "../game_components/name_resolver.h"
#include "condition.h"
#include <unordered_set>
#include <memory>

class game_state;

enum class action_type
{
    // Switches
    SWITCH_T,

    SWITCHES_END, // Do not change this

    // Modifiers
    OFF_T,
    ASSIGNMENT_T,
    INCREMENTATION_T,
    DECREMENTATION_T,
    LAZY_T,

    MODIFIERS_END, // Do not change this

    // Rest of the actions
    EMPTY_T,
    SHIFT_T,
    ON_T,
    CONDITION_T,
};


class action_result
{
    bool success;
    union
    {
        token_id_t information;
        token_id_t revert_piece;
        token_id_t revert_player;
        int revert_value;
    };
public:
    action_result(bool success)
            : success(success), information(0)
    {}

    action_result(bool success, token_id_t information)
            : success(success), information(information)
    {}

    action_result(bool success, int information)
            : success(success), revert_value(information)
    {}
    operator bool()
    {
        return success;
    }

    token_id_t get_revert_piece() const
    {
        return revert_piece;
    }

    token_id_t get_revert_player() const
    {
        return revert_player;
    }

    int get_revert_value() const
    {
        return revert_value;
    }
};

class action {
protected:
    uint index;
    action_type type;
    action(unsigned int action_index, action_type type)
        : index(action_index), type(type)
    {
    }

    explicit action(action_type type) : index(0), type(type) {}
public:
    virtual action_result apply(game_state *b) const =0;
    virtual void revert(game_state *,const action_result&) const {}
    bool is_modifier() const { return type < action_type :: MODIFIERS_END; }
    bool is_switch() const { return type < action_type :: SWITCHES_END; }
    action_type get_type() const { return type; }
    unsigned int get_index() const
    {
        return index;
    }
    virtual ~action()=default;
};

class action_application
{
    size_t applied_x,applied_y;
    const action* applied_action;
public:
    action_application(size_t x, size_t y,const action *action) : applied_x(x), applied_y(y), applied_action(action) {}

    size_t x() const
    {
        return applied_x;
    }

    size_t y() const
    {
        return applied_y;
    }

    const action* get_action() const
    {
        return applied_action;
    }
};

namespace actions
{
    class empty : public action
    {
    public:
        empty() : action(action_type :: EMPTY_T) {};
        action_result apply(game_state *) const override { return true; };
    };

    class shift : public action
    {
        long long dx, dy;
    public:
        shift(long long dx, long long dy) : action(action_type :: SHIFT_T), dx(dx), dy(dy) {}
        action_result apply(game_state *b) const override;
        void revert(game_state *b,const action_result& apply_result) const override;
        long long get_dx() const
        {
            return dx;
        }
        long long get_dy() const
        {
            return dy;
        }
    };

    class on : public action
    {
        std::vector<bool> pieces;
    public:
        explicit on(std::vector<bool> pieces)
                : action(action_type :: ON_T), pieces(std::move(pieces))
        {}
        action_result apply(game_state *b) const override;
        const std::vector<bool>& get_pieces() const
        {
            return pieces;
        }
    };

    class condition_action : public action
    {
        std::unique_ptr<condition> cond;
    public:
        explicit condition_action(std::unique_ptr<condition> cond)
                : action(action_type::CONDITION_T), cond(std::move(cond))
        {}
        action_result apply(game_state *b) const override
        {
            return cond->check(b);
        }
        const condition* get_cond() const
        {
            return cond.get();
        }
    };

    class off : public action
    {
        token_id_t piece;
    public:
        off(unsigned int index, token_id_t piece) : action(index, action_type::OFF_T), piece(piece) {}
        action_result apply(game_state *b) const override;
        void revert(game_state *b,const action_result& apply_result) const override;
        token_id_t get_piece() const
        {
            return piece;
        }
    };

    class player_switch : public action
    {
        token_id_t player;
    public:
        player_switch(unsigned int index, token_id_t player) : action(index, action_type::SWITCH_T), player(player) {}
        action_result apply(game_state *b) const override;
        void revert(game_state *b,const action_result& apply_result) const override;
        token_id_t get_player() const
        {
            return player;
        }
    };

    class assignment : public action
    {
        token_id_t variable;
        std::unique_ptr<arithmetic_operation> value;
    public:
        assignment(unsigned int index, size_t variable, std::unique_ptr<arithmetic_operation> value)
                : action(index, action_type::ASSIGNMENT_T), variable(variable), value(std::move(value)) {}
        action_result apply(game_state *b) const override;
        void revert(game_state *b,const action_result& apply_result) const override;
        token_id_t get_variable() const
        {
            return variable;
        }
        const arithmetic_operation* get_value_expression() const
        {
            return value.get();
        }
    };

    class incrementation : public action
    {
        token_id_t variable;
    public:
        incrementation(unsigned int index, size_t variable)
                : action(index, action_type::INCREMENTATION_T), variable(variable)
        {}
        action_result apply(game_state *b) const override;
        void revert(game_state *b,const action_result& apply_result) const override;
        token_id_t get_variable() const
        {
            return variable;
        }
    };

    class decrementation : public action
    {
        token_id_t variable;
    public:
        decrementation(unsigned int index, size_t variable)
                : action(index, action_type::DECREMENTATION_T), variable(variable)
        {}
        action_result apply(game_state *b) const override;
        void revert(game_state *b,const action_result& apply_result) const override;
        token_id_t get_variable() const
        {
            return variable;
        }
    };

    class lazy : public action
    {
        action* lazy_action;
    public:
        lazy(unsigned int index, action* lazy_action) : action(index, action_type::LAZY_T), lazy_action(lazy_action) {}
        action_result apply(game_state *b) const override;
        void revert(game_state *b,const action_result& apply_result) const override;
        const action* get_action() const
        {
            return lazy_action;
        }
    };
}




#endif //RBGGAMEMANAGER_ACTION_H
