//
// Created by shrum on 28.12.17.
//

#ifndef RBGGAMEMANAGER_ACTION_H
#define RBGGAMEMANAGER_ACTION_H

#include <vector>
#include <unordered_set>
#include <memory>
#include "rbg2gdl/src/condition_check.hpp"


class board;
class game_state;


class action {
    int index;
public:
    action(int index) : index(index) {}
    virtual bool apply(game_state *b)=0;
};

namespace actions
{
    class shift : public action {
        int dx, dy;
    public:
        shift(int index, int dx, int dy) : action(index), dx(dx), dy(dy) {}
        bool apply(game_state *b) override;
    };

    class on : public action {
        std::unordered_set<int> pieces;
    public:
        on(int index, const std::vector<int>& pieces)
                : action(index), pieces(pieces.begin(),pieces.end()) {}
        bool apply(game_state *b) override;
    };

    class abstract_condition {
    public:
        virtual bool check(game_state *b)=0;
    };

    class condition_action : public action {
        std::unique_ptr<abstract_condition> cond;
    public:
        condition_action(int index,const condition_check& check) : action(index)
        {
            /* TODO(shrum): create condition out of this condition check. Probably by some other visitor. */
        }
        bool apply(game_state *b) override;
    };

    namespace conditions {

        class conjunction : public abstract_condition {
            std::unique_ptr<abstract_condition> left, right;
        public:
            bool check(game_state *b) override;
        };

        class alternative : public abstract_condition {
            std::unique_ptr<abstract_condition> left, right;
        public:
            bool check(game_state *b) override;
        };

        class negation : public abstract_condition {
            std::unique_ptr<abstract_condition> cond;
        public:
            bool check(game_state *b) override;
        };

        class arithmetic_operation {
        public:
            virtual int value(game_state *b)=0;
        };

        class less : public abstract_condition {
            std::unique_ptr<arithmetic_operation> left, right;
        public:
            bool check(game_state *b) override;
        };

        class less_equal : public abstract_condition {
            std::unique_ptr<arithmetic_operation> left, right;
        public:
            bool check(game_state *b) override;
        };

        class equal : public abstract_condition {
            std::unique_ptr<arithmetic_operation> left, right;
        public:
            bool check(game_state *b) override;
        };

        namespace arithmetic {
            class variable : public arithmetic_operation {
                int variable_index;
            public:
                int value(game_state *b) override;
            };

            class constant : public arithmetic_operation {
                int constant_value;
            public:
                int value(game_state *b) override;
            };
        }

        class move_pattern : public condition {
            /* TODO(shrum): store nfa, evaluated stuff and other? */
        };
    }

    class off : public action {
        int piece_id;
    public:
        off(int index, int piece_id) : action(index), piece_id(piece_id) {}
        bool apply(game_state *b) override;
    };

    class lazy_off : public action {
        int piece_id;
    public:
        lazy_off(int index, int piece_id) : action(index), piece_id(piece_id) {}
        bool apply(game_state *b) override;
    };

    class player_switch : public action {
        int player_id;
    public:
        player_switch(int index, int player_id) : action(index), player_id(player_id) {}
        bool apply(game_state *b) override;
    };

    class semi_switch : public action {
    public:
        semi_switch(int index) : action(index) {}
        bool apply(game_state *b) override;
    };

    class assignment : public action {
        int variable;
        int value;
    public:
        assignment(int index, int variable, int value) : action(index), variable(variable), value(value) {}
        bool apply(game_state *b) override;
    };

    class lazy_assignment : public action {
        int variable;
        int value;
    public:
        lazy_assignment(int index, int variable, int value) : action(index), variable(variable), value(value) {}
        bool apply(game_state *b) override;
    };
}



#endif //RBGGAMEMANAGER_ACTION_H
