//
// Created by shrum on 02.01.18.
//

#include "dispatchers.h"

void game_nfa_dispatcher::dispatch(const rbg_parser::sum &move) {
    fsm::state_id_t new_initial_id = result.new_state();
    result.set_initial(new_initial_id);
    fsm::state_id_t new_final_id = result.new_state();
    result.set_final(new_final_id);

    fsm::nfa<action> current = result;
    for(const std::unique_ptr<rbg_parser::game_move> & child : move.get_content())
    {
        child->accept(*this);
        current.initial().add_transition(result.get_initial_id());
        result.final().add_transition(current.get_final_id());
    }
    result = current;
}

void game_nfa_dispatcher::dispatch(const rbg_parser::pure_sum& move) {
    fsm::state_id_t new_initial_id = result.new_state();
    result.set_initial(new_initial_id);
    fsm::state_id_t new_final_id = result.new_state();
    result.set_final(new_final_id);

    fsm::nfa<action> current = result;
    for(const std::unique_ptr<rbg_parser::pure_game_move> & child : move.get_content())
    {
        child->accept(*this);
        current.initial().add_transition(result.get_initial_id());
        result.final().add_transition(current.get_final_id());
    }
    result = current;
}

void game_nfa_dispatcher::dispatch(const rbg_parser::concatenation& move) {
    move.get_content().front()->accept(*this);

    fsm::nfa<action> current = result;
    for(size_t i = 1; i < move.get_content().size(); i++)
    {
        const auto& child = move.get_content()[i];
        child->accept(*this);
        current.final().add_transition(result.get_initial_id());
        current.set_final(result.get_final_id());
    }
    result = current;
}

void game_nfa_dispatcher::dispatch(const rbg_parser::pure_concatenation& move) {
    move.get_content().front()->accept(*this);

    fsm::nfa<action> current = result;
    for(size_t i = 1; i < move.get_content().size(); i++)
    {
        const auto& child = move.get_content()[i];
        child->accept(*this);
        current.final().add_transition(result.get_initial_id());
        current.set_final(result.get_final_id());
    }
    result = current;
}

void game_nfa_dispatcher::dispatch(const rbg_parser::bracketed_move& move) {
    move.get_content()->accept(*this);
    if(move.is_star())
    {
        fsm::state_id_t new_initial_id = result.new_state();
        fsm::state_id_t new_final_id = result.new_state();

        result[new_initial_id].add_transition(result.get_initial_id());
        result[new_initial_id].add_transition(new_final_id);
        result[new_final_id].add_transition(new_initial_id);
        result.final().add_transition(new_final_id);

        result.set_initial(new_initial_id);
        result.set_final(new_final_id);
        return;
    }

    fsm::nfa<action> current = result;

    for(size_t i = 1; i < move.get_number_of_repetitions(); i++)
    {
        move.get_content()->accept(*this);
        current.final().add_transition(result.get_initial_id());
        current.set_final(result.get_final_id());
    }

    result = current;
}

void game_nfa_dispatcher::dispatch(const rbg_parser::pure_bracketed_move& move) {
    move.get_content()->accept(*this);
    if(move.is_star())
    {
        fsm::state_id_t new_initial_id = result.new_state();
        fsm::state_id_t new_final_id = result.new_state();

        result[new_initial_id].add_transition(result.get_initial_id());
        result[new_initial_id].add_transition(new_final_id);
        result[new_final_id].add_transition(new_initial_id);
        result.final().add_transition(new_final_id);

        result.set_initial(new_initial_id);
        result.set_final(new_final_id);
        return;
    }

    fsm::nfa<action> current = result;

    for(size_t i = 1; i < move.get_number_of_repetitions(); i++)
    {
        move.get_content()->accept(*this);
        current.final().add_transition(result.get_initial_id());
        current.set_final(result.get_final_id());
    }

    result = current;
}

void game_nfa_dispatcher::dispatch(const rbg_parser::shift& move) {
    fsm::state_id_t initial_id = result.new_state();
    fsm::state_id_t final_id = result.new_state();
    result.set_initial(initial_id);
    result.set_final(final_id);
    result.initial().add_transition(final_id,std::unique_ptr<action>(new actions::shift(move.get_x(),move.get_y())));
}

void game_nfa_dispatcher::dispatch(const rbg_parser::ons & move) {
    fsm::state_id_t initial_id = result.new_state();
    fsm::state_id_t final_id = result.new_state();
    result.set_initial(initial_id);
    result.set_final(final_id);
    std::vector<token_id_t> pieces;
    for(const auto& piece_token : move.get_legal_ons())
    {
        pieces.emplace_back(resolver.get_piece_id(piece_token.to_string()));
    }
    result.initial().add_transition(final_id,std::unique_ptr<action>(new actions::on(pieces)));
}

void game_nfa_dispatcher::dispatch(const rbg_parser::off & move) {
    fsm::state_id_t initial_id = result.new_state();
    fsm::state_id_t final_id = result.new_state();
    result.set_initial(initial_id);
    result.set_final(final_id);
    token_id_t piece_id = resolver.get_piece_id(move.get_piece().to_string());
    if(move.is_lazy())
    {
        result.initial().add_transition(final_id, std::unique_ptr<action>(new actions::lazy<actions::modifiers::off>(modifier_states.size(),piece_id)));
    }
    else
    {
        result.initial().add_transition(final_id, std::unique_ptr<action>(new actions::modifiers::off(modifier_states.size(),piece_id)));
    }
    modifier_states.push_back(initial_id);
}

void game_nfa_dispatcher::dispatch(const rbg_parser::assignment & move) {
    fsm::state_id_t initial_id = result.new_state();
    fsm::state_id_t final_id = result.new_state();
    result.set_initial(initial_id);
    result.set_final(final_id);
    const std::string& variable_name = move.get_left_side().to_string();
    token_id_t variable_id = resolver.get_variable_id(variable_name);
    int value = move.get_right_side().get_value();
    if(move.is_lazy())
    {
        result.initial().add_transition(final_id,std::unique_ptr<action>(new actions::lazy<actions::modifiers::assignment>(modifier_states.size(), variable_id, value)));
    }
    else
    {
        result.initial().add_transition(final_id,std::unique_ptr<action>(new actions::modifiers::assignment(modifier_states.size(), variable_id, value)));
    }
    modifier_states.push_back(initial_id);
}

void game_nfa_dispatcher::dispatch(const rbg_parser::player_switch & move) {
    fsm::state_id_t initial_id = result.new_state();
    fsm::state_id_t final_id = result.new_state();
    result.set_initial(initial_id);
    result.set_final(final_id);
    if(move.changes_player()) {
        const std::string& player_name = move.get_player().to_string();
        token_id_t player_id = resolver.get_player_id(player_name);
        result.initial().add_transition(final_id, std::unique_ptr<action>(new actions::modifiers::switches::player_switch(modifier_states.size(), player_id)));
    }
    else
    {
        result.initial().add_transition(final_id, std::unique_ptr<action>(new actions::modifiers::switches::semi_switch(modifier_states.size())));
    }
    modifier_states.push_back(initial_id);
}

void game_nfa_dispatcher::dispatch(const rbg_parser::condition_check & move) {
    fsm::state_id_t initial_id = result.new_state();
    fsm::state_id_t final_id = result.new_state();
    result.set_initial(initial_id);
    result.set_final(final_id);
    result.initial().add_transition(final_id); /* TODO(shrum): Conditions */
}
