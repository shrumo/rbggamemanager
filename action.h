//
// Created by shrum on 28.12.17.
//

#ifndef RBGGAMEMANAGER_ACTION_H
#define RBGGAMEMANAGER_ACTION_H

#include <vector>
#include <unordered_map>
#include <memory>
#include "rbg2gdl/src/condition_check.hpp"
#include "game_state.h"
#include "automaton.h"

class action {
protected:
public:
    virtual bool apply(game_state *b)=0;
    virtual void revert(game_state *b)=0;
    virtual bool is_modifier() const { return false; }
    virtual bool is_switch() const { return false; }
};

namespace actions
{
    class shift : public action {
        long long dx, dy;
    public:
        shift(long long dx, long long dy) : dx(dx), dy(dy) {}
        bool apply(game_state *b) override;
        void revert(game_state *b) override;
    };

    class on : public action {
        std::unordered_set<token_id_t> pieces;
    public:
        on(const std::vector<token_id_t>& pieces)
                : pieces(pieces.begin(),pieces.end()) {}
        bool apply(game_state *b) override;
        void revert(game_state *b) override;
    };

    class condition {
    public:
        virtual bool check(game_state *b)=0;
    };

    class condition_action : public action {
        std::unique_ptr<condition> cond;
    public:
        condition_action(std::unique_ptr<condition> cond)
            : cond(std::move(cond))
        {
            /* TODO(shrum): create condition out of this condition check. Probably by some other visitor. */
        }
        bool apply(game_state *b) override;
        void revert(game_state *b) override;
    };

    namespace conditions {
        class conjunction : public condition {
            std::vector<std::unique_ptr<condition> > items;
        public:
            conjunction(std::vector<std::unique_ptr<condition> > items)
                    : items(std::move(items))
            {}
            bool check(game_state *b) override;
        };

        class alternative : public condition {
            std::vector<std::unique_ptr<condition> > items;
        public:
            alternative(std::vector<std::unique_ptr<condition> > items)
            : items(std::move(items))
                    {}
            bool check(game_state *b) override;
        };

        class negation : public condition {
            std::unique_ptr<condition> cond;
        public:
            negation(std::unique_ptr<condition> cond)
                : cond(std::move(cond))
            {}
            bool check(game_state *b) override;
        };

        class arithmetic_operation {
        public:
            virtual int value(game_state *b)=0;
        };

        class less : public condition {
            std::unique_ptr<arithmetic_operation> left, right;
        public:
            less(std::unique_ptr<arithmetic_operation> left, std::unique_ptr<arithmetic_operation> right)
                : left(std::move(left)), right(std::move(right))
                {}
            bool check(game_state *b) override;
        };

        class less_equal : public condition {
            std::unique_ptr<arithmetic_operation> left, right;
        public:
            less_equal(std::unique_ptr<arithmetic_operation> left, std::unique_ptr<arithmetic_operation> right)
            : left(std::move(left)), right(std::move(right))
                {}
            bool check(game_state *b) override;
        };

        class equal : public condition {
            std::unique_ptr<arithmetic_operation> left, right;
        public:
            equal(std::unique_ptr<arithmetic_operation> left, std::unique_ptr<arithmetic_operation> right)
            : left(std::move(left)), right(std::move(right))
                {}
            bool check(game_state *b) override;
        };

        class not_equal : public condition {
            std::unique_ptr<arithmetic_operation> left, right;
        public:
            not_equal(std::unique_ptr<arithmetic_operation> left, std::unique_ptr<arithmetic_operation> right)
                    : left(std::move(left)), right(std::move(right))
            {}
            bool check(game_state *b) override;
        };

        namespace arithmetic {
            class variable : public arithmetic_operation {
                token_id_t variable_index;
            public:
                variable(token_id_t variable_index)
                        : variable_index(variable_index)
                {}
                int value(game_state *b) override;
            };

            class constant : public arithmetic_operation {
                int constant_value;
            public:
                constant(int constant_value)
                        : constant_value(constant_value)
                {}
                int value(game_state *b) override;
            };
        }

        class move_pattern : public condition {
            fsm::nfa<action> move_pattern_nfa;
            std::unique_ptr<std::unordered_map<game_state_identifier,bool,identifier_hash>> visited;
            size_t visited_moves_count;
            std::unique_ptr<fsm::state_register<action> > state_register;
        public:
            move_pattern(const fsm::nfa<action>& move_nfa, std::unique_ptr<fsm::state_register<action> > state_register)
                    : move_pattern_nfa(move_nfa), visited_moves_count(0), visited(nullptr), state_register(std::move(state_register))
            {
            }
            bool check(game_state *b) override;
            bool check_play_exists_rec(size_t nfa_state, game_state* state, game_move* current_move);
        };
    }

    class modifier : public action
    {
        size_t index;
        size_t lazy_head_before;
    public:
        modifier(size_t index) : index(index)
        {}
        virtual bool apply(game_state *b) override;
        virtual void revert(game_state *b) override;
        bool is_modifier() const override
        {
            return true;
        }
        size_t get_index() const {
            return index;
        }
    };

    class switch_action : public modifier {
    public:
        switch_action(size_t index) : modifier(index) {}
        bool is_switch() const override
        {
            return true;
        }
    };

    template<typename Modifier,
            typename = std::enable_if<not std::is_base_of<switch_action,Modifier>::value> >
    class lazy : public modifier {
    private:
        std::unique_ptr<Modifier> lazy_action;
    public:
        template<typename ...Args>
        lazy(size_t index, Args... args) : modifier(index), lazy_action(new Modifier(index, args...))
        {}
        bool apply(game_state *b) override {
            b->add_lazy_action(lazy_action.get());
            return true;
        }
        void revert(game_state *b) override
        {
            b->pop_lazy_action();
        }
    };

    namespace modifiers
    {
        class off : public modifier {
            token_id_t piece_id;
            token_id_t previous_piece_id;
        public:
            off(size_t index, token_id_t piece_id) : modifier(index), piece_id(piece_id) {}
            bool apply(game_state *b) override;
            void revert(game_state *b) override;
        };

        namespace switches {
            class player_switch : public switch_action {
                token_id_t player_id;
                token_id_t previous_player_id;
            public:
                player_switch(size_t index, token_id_t player_id) : switch_action(index), player_id(player_id) {}

                bool apply(game_state *b) override;
                void revert(game_state *b) override;

                bool is_switch() const override {
                    return true;
                }
            };

            class semi_switch : public switch_action {
            public:
                semi_switch(size_t index) : switch_action(index) {}

                bool apply(game_state *b) override;
                void revert(game_state *b) override;

                bool is_switch() const override {
                    return true;
                }
            };
        }

        class assignment : public modifier {
            token_id_t variable;
            int value;
            int previous_value;
        public:
            assignment(size_t index, token_id_t variable, int value) : modifier(index), variable(variable), value(value) {}
            bool apply(game_state *b) override;
            void revert(game_state *b) override;
        };
    }

}


#endif //RBGGAMEMANAGER_ACTION_H
