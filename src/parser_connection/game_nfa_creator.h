//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_GAME_NFA_CREATOR_H
#define RBGGAMEMANAGER_GAME_NFA_CREATOR_H

#include "../../rbgParser/src/sum.hpp"
#include "../../rbgParser/src/pure_sum.hpp"
#include "../../rbgParser/src/concatenation.hpp"
#include "../../rbgParser/src/pure_concatenation.hpp"
#include "../../rbgParser/src/bracketed_move.hpp"
#include "../../rbgParser/src/pure_bracketed_move.hpp"
#include "../../rbgParser/src/shift.hpp"
#include "../../rbgParser/src/ons.hpp"
#include "../../rbgParser/src/offs.hpp"
#include "../../rbgParser/src/assignments.hpp"
#include "../../rbgParser/src/switch.hpp"
#include "../../rbgParser/src/condition_check.hpp"
#include "../../rbgParser/src/conjunction.hpp"
#include "../../rbgParser/src/alternative.hpp"
#include "../../rbgParser/src/negatable_condition.hpp"
#include "../../rbgParser/src/comparison.hpp"
#include "../../rbgParser/src/move_condition.hpp"
#include "../../rbgParser/src/modifier_block.hpp"
#include "../actions/action.h"
#include "../game_nfa/game_moves_description.h"

struct recover_information
{
    std::unique_ptr<fsm::nfa<action*>> nfa_result;
    bool register_modifiers;
    bool reuse_final_as_intial;
    fsm::state_id_t last_final;
    recover_information(std::unique_ptr<fsm::nfa<action*>> nfa_result, bool register_modifiers,
                        bool reuse_final_as_intial, fsm::state_id_t last_final)
            : nfa_result(std::move(nfa_result)), register_modifiers(register_modifiers),
              reuse_final_as_intial(reuse_final_as_intial), last_final(last_final)
    {}
};

class game_nfa_creator : public rbg_parser::abstract_dispatcher
{
    const name_resolver& resolver;
    std::unique_ptr<fsm::nfa<action*>> nfa_result;
    std::unique_ptr<condition> condition_result;
    std::unordered_map<std::string, action*> used_actions;

    std::vector<std::unique_ptr<action> > actions;

    bool block_started;
    std::vector<fsm::state_id_t> blocks_states;
    bool register_modifiers;

    bool reuse_final_as_initial;
    fsm::state_id_t last_final;

    void register_modifier(fsm::state_id_t initial_id);
    void start_block();
    void stop_block();

    unsigned int move_pattern_count;

    recover_information start_move_pattern()
    {
        recover_information information(std::move(nfa_result), register_modifiers, reuse_final_as_initial, last_final);
        register_modifiers = false;
        nfa_result = std::unique_ptr<fsm::nfa<action*>>(new fsm::nfa<action*>());
        last_final = 0;
        reuse_final_as_initial = false;
        return information;
    }

    void stop_move_pattern(recover_information information)
    {
        nfa_result = std::move(information.nfa_result);
        register_modifiers = information.register_modifiers;
        reuse_final_as_initial = information.reuse_final_as_intial;
        last_final = information.last_final;
    }

    game_nfa_creator(const name_resolver& resolver)
            : resolver(resolver), nfa_result(new fsm::nfa<action*>()),
              block_started(false), reuse_final_as_initial(false), last_final(0),
              move_pattern_count(0)
    {
        blocks_states.push_back(0);
    }

    void stop_reusing_finals()
    {
        reuse_final_as_initial = false;
    }

    void start_reusing_finals()
    {
        reuse_final_as_initial = true;
    }

    fsm::state_id_t new_initial()
    {
        if(reuse_final_as_initial) {
            reuse_final_as_initial = false;
            return last_final;
        }
        return nfa_result->new_state();
    }

public:
    void dispatch(const rbg_parser::sum&) override;
    void dispatch(const rbg_parser::pure_sum&) override;
    void dispatch(const rbg_parser::concatenation&) override;
    void dispatch(const rbg_parser::pure_concatenation&) override;
    void dispatch(const rbg_parser::bracketed_move&) override;
    void dispatch(const rbg_parser::pure_bracketed_move&) override;
    void dispatch(const rbg_parser::shift&) override;
    void dispatch(const rbg_parser::ons&) override;
    void dispatch(const rbg_parser::off&) override;
    void dispatch(const rbg_parser::assignment&) override;
    void dispatch(const rbg_parser::player_switch&) override;
    void dispatch(const rbg_parser::condition_check&) override;
    void dispatch(const rbg_parser::conjunction&) override;
    void dispatch(const rbg_parser::alternative&) override;
    void dispatch(const rbg_parser::negatable_condition&) override;
    void dispatch(const rbg_parser::comparison&) override;
    void dispatch(const rbg_parser::move_condition&) override;
    void dispatch(const rbg_parser::modifier_block&) override;

    std::unique_ptr<fsm::nfa<action*>> get_nfa()
    {
        return std::move(nfa_result);
    }

    std::unique_ptr<condition> get_condition()
    {
        return std::move(condition_result);
    }

    game_moves_description create_description()
    {
        return game_moves_description(get_nfa(), std::move(blocks_states), std::move(actions), move_pattern_count);
    }

    friend game_moves_description create_moves(const rbg_parser::game_move& move, const name_resolver& resolver);
};


#endif //RBGGAMEMANAGER_GAME_NFA_CREATOR_H
