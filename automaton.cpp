//
// Created by shrum on 27.12.17.
//

#include "automaton.h"
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
#include "game.h"


void game_nfa_creator::dispatch(const rbg_parser::sum &move) {
    int new_initial_id = result.new_state();
    result.set_initial(new_initial_id);
    int new_final_id = result.new_state();
    result.set_final(new_final_id);

    fsm::nfa current = result;
    for(const std::unique_ptr<rbg_parser::game_move> & child : move.get_content())
    {
        child->accept(*this);
        current.initial_state().add_transition(result.get_initial_id());
        result.final_state().add_transition(current.get_final_id());
    }
    result = current;
}

void game_nfa_creator::dispatch(const rbg_parser::pure_sum& move) {
    int new_initial_id = result.new_state();
    result.set_initial(new_initial_id);
    int new_final_id = result.new_state();
    result.set_final(new_final_id);

    fsm::nfa current = result;
    for(const std::unique_ptr<rbg_parser::pure_game_move> & child : move.get_content())
    {
        child->accept(*this);
        current.initial_state().add_transition(result.get_initial_id());
        result.final_state().add_transition(current.get_final_id());
    }
    result = current;
}

void game_nfa_creator::dispatch(const rbg_parser::concatenation& move) {
    move.get_content().front()->accept(*this);

    fsm::nfa current = result;
    for(int i = 1; i < move.get_content().size(); i++)
    {
        const auto& child = move.get_content()[i];
        child->accept(*this);
        current.final_state().add_transition(result.get_initial_id());
        current.set_final(result.get_final_id());
    }
    result = current;
}

void game_nfa_creator::dispatch(const rbg_parser::pure_concatenation& move) {
    move.get_content().front()->accept(*this);

    fsm::nfa current = result;
    for(int i = 1; i < move.get_content().size(); i++)
    {
        const auto& child = move.get_content()[i];
        child->accept(*this);
        current.final_state().add_transition(result.get_initial_id());
        current.set_final(result.get_final_id());
    }
    result = current;
}

void game_nfa_creator::dispatch(const rbg_parser::bracketed_move& move) {
    move.get_content()->accept(*this);
    if(move.is_star())
    {
        int new_initial_id = result.new_state();
        int new_final_id = result.new_state();

        result[new_initial_id].add_transition(result.get_initial_id());
        result[new_initial_id].add_transition(new_final_id);
        result[new_final_id].add_transition(new_initial_id);
        result.final_state().add_transition(new_final_id);

        result.set_initial(new_initial_id);
        result.set_final(new_final_id);
        return;
    }

    fsm::nfa current = result;

    for(int i = 1; i < move.get_number_of_repetitions(); i++)
    {
        move.get_content()->accept(*this);
        current.final_state().add_transition(result.get_initial_id());
        current.set_final(result.get_final_id());
    }

    result = current;
}

void game_nfa_creator::dispatch(const rbg_parser::pure_bracketed_move& move) {
    move.get_content()->accept(*this);
    if(move.is_star())
    {
        int new_initial_id = result.new_state();
        int new_final_id = result.new_state();

        result[new_initial_id].add_transition(result.get_initial_id());
        result[new_initial_id].add_transition(new_final_id);
        result[new_final_id].add_transition(new_initial_id);
        result.final_state().add_transition(new_final_id);

        result.set_initial(new_initial_id);
        result.set_final(new_final_id);
        return;
    }

    fsm::nfa current = result;

    for(int i = 1; i < move.get_number_of_repetitions(); i++)
    {
        move.get_content()->accept(*this);
        current.final_state().add_transition(result.get_initial_id());
        current.set_final(result.get_final_id());
    }

    result = current;
}

void game_nfa_creator::dispatch(const rbg_parser::shift& move) {
    int initial_id = result.new_state();
    int final_id = result.new_state();
    result.set_initial(initial_id);
    result.set_final(final_id);
    result.initial_state().add_transition(final_id, new actions::shift(counter,move.get_x(),move.get_y()));
    counter++;
}

void game_nfa_creator::dispatch(const rbg_parser::ons & move) {
    int initial_id = result.new_state();
    int final_id = result.new_state();
    result.set_initial(initial_id);
    result.set_final(final_id);
    std::vector<int> pieces;
    for(const auto& piece_token : move.get_legal_ons())
    {
        pieces.emplace_back(resolver.pieces_ids.at(piece_token.to_string()));
    }
    result.initial_state().add_transition(final_id, new actions::on(counter, pieces));
    counter++;
}

void game_nfa_creator::dispatch(const rbg_parser::off & move) {
    int initial_id = result.new_state();
    int final_id = result.new_state();
    result.set_initial(initial_id);
    result.set_final(final_id);
    result.initial_state().add_transition(final_id, new actions::modifiers::off(counter,resolver.pieces_ids.at(move.get_piece().to_string())));
    counter++;
}

void game_nfa_creator::dispatch(const rbg_parser::assignment & move) {
    int initial_id = result.new_state();
    int final_id = result.new_state();
    result.set_initial(initial_id);
    result.set_final(final_id);
    std::string variable_name = move.get_left_side().to_string();
    int variable_id = resolver.variables_ids.at(variable_name);
    int value = move.get_right_side().get_value();
    result.initial_state().add_transition(final_id, new actions::modifiers::assignment(counter, variable_id, value));
    counter++;
}

void game_nfa_creator::dispatch(const rbg_parser::player_switch & move) {
    int initial_id = result.new_state();
    int final_id = result.new_state();
    result.set_initial(initial_id);
    result.set_final(final_id);
    if(move.changes_player()) {
        std::string player_name = move.get_player().to_string();
        int player_id = resolver.players_ids.at(player_name);
        result.initial_state().add_transition(final_id, new actions::modifiers::player_switch(counter, player_id));
    }
    else
    {
        result.initial_state().add_transition(final_id, new actions::modifiers::semi_switch(counter));
    }
    counter++;
}

void game_nfa_creator::dispatch(const rbg_parser::condition_check & move) {
    int initial_id = result.new_state();
    int final_id = result.new_state();
    result.set_initial(initial_id);
    result.set_final(final_id);
    result.initial_state().add_transition(final_id, new actions::condition_action(counter, move));
    counter++;
}

void game_nfa_creator::dispatch(const rbg_parser::conjunction &) {

}

void game_nfa_creator::dispatch(const rbg_parser::alternative &) {

}

void game_nfa_creator::dispatch(const rbg_parser::negatable_condition &) {

}

void game_nfa_creator::dispatch(const rbg_parser::comparison &) {

}

void game_nfa_creator::dispatch(const rbg_parser::move_condition &) {

}

