//
// Created by shrum on 02.01.18.
//

#ifndef RBGGAMEMANAGER_EXPRESSION_TRAVERSE_H
#define RBGGAMEMANAGER_EXPRESSION_TRAVERSE_H

#include "automaton.h"
#include "rbg2gdl/src/abstract_dispatcher.hpp"
#include "name_resolver.h"

#include "rbg2gdl/src/sum.hpp"
#include "rbg2gdl/src/pure_sum.hpp"
#include "rbg2gdl/src/concatenation.hpp"
#include "rbg2gdl/src/pure_concatenation.hpp"
#include "rbg2gdl/src/bracketed_move.hpp"
#include "rbg2gdl/src/pure_bracketed_move.hpp"
#include "rbg2gdl/src/shift.hpp"
#include "rbg2gdl/src/ons.hpp"
#include "rbg2gdl/src/offs.hpp"
#include "rbg2gdl/src/assignments.hpp"
#include "rbg2gdl/src/switch.hpp"
#include "rbg2gdl/src/condition_check.hpp"
#include "rbg2gdl/src/conjunction.hpp"
#include "rbg2gdl/src/alternative.hpp"
#include "rbg2gdl/src/negatable_condition.hpp"
#include "rbg2gdl/src/comparison.hpp"
#include "rbg2gdl/src/move_condition.hpp"
#include "action.h"

class game_nfa_dispatcher : public rbg_parser::abstract_dispatcher{
    std::vector<fsm::state_id_t> modifier_states;
    fsm::nfa<action> result;
    const name_resolver& resolver;
public:
    game_nfa_dispatcher(fsm::state_register<action>* ref, const name_resolver& resolver)
            :result(ref), resolver(resolver), modifier_states(1, 0)
    {
    }

    const fsm::nfa<action>& get_result() const
    {
        return result;
    }

    const std::vector<fsm::state_id_t>& get_modifier_states() const
    {
        return modifier_states;
    }

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
    void dispatch(const rbg_parser::conjunction&) override {};
    void dispatch(const rbg_parser::alternative&) override {};
    void dispatch(const rbg_parser::negatable_condition&) override {};
    void dispatch(const rbg_parser::comparison&) override {};
    void dispatch(const rbg_parser::move_condition&) override {};
};

#endif //RBGGAMEMANAGER_EXPRESSION_TRAVERSE_H
