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
    int index;
    action(unsigned int action_index, bool is_switch)
    {
            if(is_switch)
                index = - (int) action_index;
            else
                index = (int) action_index;
    }
public:
    action() : index(0) {}
    virtual action_result apply(game_state *b) const =0;
    virtual void revert(game_state *b,const action_result& apply_result) const {}
    bool is_modifier() const { return index != 0; }
    bool is_switch() const { return index < 0; }
    unsigned int get_index()
    {
        return (unsigned int) std::abs(index);
    }
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
        action_result apply(game_state *b) const override { return true; };
    };

    class shift : public action
    {
        long long dx, dy;
    public:
        shift(long long dx, long long dy) : dx(dx), dy(dy) {}
        action_result apply(game_state *b) const override;
        void revert(game_state *b,const action_result& apply_result) const override;
    };

    class on : public action
    {
        std::vector<bool> pieces;
    public:
        on(std::vector<bool> pieces)
                : pieces(std::move(pieces))
        {}
        action_result apply(game_state *b) const override;
    };

    class condition_action : public action
    {
        std::unique_ptr<condition> cond;
    public:
        condition_action(std::unique_ptr<condition> cond)
                : cond(std::move(cond))
        {}
        action_result apply(game_state *b) const override
        {
            return cond->check(b);
        }
    };

    class off : public action
    {
        token_id_t piece;
    public:
        off(unsigned int index, token_id_t piece) : action(index, false), piece(piece) {}
        action_result apply(game_state *b) const override;
        void revert(game_state *b,const action_result& apply_result) const override;
    };

    class player_switch : public action
    {
        token_id_t player;
    public:
        player_switch(unsigned int index, token_id_t player) : action(index,true), player(player) {}
        action_result apply(game_state *b) const override;
        virtual void revert(game_state *b,const action_result& apply_result) const override;
    };

    class semi_switch : public action
    {
    public:
        semi_switch(unsigned int index) : action(index, true) {}
        action_result apply(game_state *b) const override { return true; };
    };

    class assignment : public action
    {
        token_id_t variable;
        int value;
    public:
        assignment(unsigned int index, size_t variable, int value) : action(index, false), variable(variable), value(value) {}
        action_result apply(game_state *b) const override;
        virtual void revert(game_state *b,const action_result& apply_result) const override;
    };

    class lazy : public action
    {
        action* lazy_action;
    public:
        lazy(unsigned int index, action* lazy_action) : action(index, false), lazy_action(lazy_action) {}
        action_result apply(game_state *b) const override;
        void revert(game_state *b,const action_result& apply_result) const override;
    };
}




#endif //RBGGAMEMANAGER_ACTION_H
