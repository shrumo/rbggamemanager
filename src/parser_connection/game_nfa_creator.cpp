//
// Created by shrum on 12.01.18.
//

#include <cursesm.h>
#include "game_nfa_creator.h"


void game_nfa_creator::dispatch(const rbg_parser::conjunction& move) {
    std::vector<std::unique_ptr<condition> > rec_result;
    for(const auto& item : move.get_content())
    {
        item->accept(*this);
        rec_result.push_back(std::move(condition_result));
    }
    condition_result = std::unique_ptr<condition>(new conditions::conjunction(std::move(rec_result)));
}

void game_nfa_creator::dispatch(const rbg_parser::alternative& move) {
    std::vector<std::unique_ptr<condition> > rec_result;
    for(const auto& item : move.get_content())
    {
        item->accept(*this);
        rec_result.push_back(std::move(condition_result));
    }
    condition_result = std::unique_ptr<condition>(new conditions::conjunction(std::move(rec_result)));
}

void game_nfa_creator::dispatch(const rbg_parser::negatable_condition& move) {
    move.get_content()->accept(*this);
    condition_result = std::unique_ptr<condition>(new conditions::negation(std::move(condition_result)));
}

void game_nfa_creator::dispatch(const rbg_parser::comparison& move) {
    std::unique_ptr<arithmetic_operation> left;
    if(move.get_left_side().get_type() == rbg_parser::number)
    {
        left = std::unique_ptr<arithmetic_operation>(new arithmetic_operations::constant_value(move.get_left_side().get_value()));
    } else
    {
        token_id_t variable_id = resolver.id(move.get_left_side().to_string());
        left = std::unique_ptr<arithmetic_operation>(new arithmetic_operations::variable_value(variable_id));
    }

    std::unique_ptr<arithmetic_operation> right;
    if(move.get_right_side().get_type() == rbg_parser::number)
    {
        right = std::unique_ptr<arithmetic_operation>(new arithmetic_operations::constant_value(move.get_right_side().get_value()));
    } else
    {
        token_id_t variable_id = resolver.id(move.get_right_side().to_string());
        right = std::unique_ptr<arithmetic_operation>(new arithmetic_operations::variable_value(variable_id));
    }

    switch (move.get_kind_of_comparison().get_type())
    {
        case rbg_parser::not_equal:
            condition_result = std::unique_ptr<condition>(new conditions::not_equal(std::move(left),std::move(right)));
            break;
        case rbg_parser::double_equal:
            condition_result = std::unique_ptr<condition>(new conditions::equal(std::move(left),std::move(right)));
            break;
        case rbg_parser::less:
            condition_result = std::unique_ptr<condition>(new conditions::less(std::move(left),std::move(right)));
            break;
        case rbg_parser::less_equal:
            condition_result = std::unique_ptr<condition>(new conditions::less_equal(std::move(left),std::move(right)));
            break;
        case rbg_parser::greater:
            condition_result = std::unique_ptr<condition>(new conditions::less(std::move(right),std::move(left)));
            break;
        case rbg_parser::greater_equal:
            condition_result = std::unique_ptr<condition>(new conditions::less_equal(std::move(right),std::move(left)));
            break;
    }
}

void game_nfa_creator::dispatch(const rbg_parser::move_condition& move) {
    auto result = start_move_pattern();
    move.get_content()->accept(*this);
    condition_result = std::unique_ptr<condition>(new conditions::move_pattern(move_pattern_count, get_nfa()));
    move_pattern_count++;
    stop_move_pattern(std::move(result));
}

void game_nfa_creator::dispatch(const rbg_parser::sum& move) {
    if(used_actions.find("epsilon") == used_actions.end())
    {
        std::unique_ptr<action> empty_action(new actions::empty());
        used_actions["epsilon"] = empty_action.get();
        actions.push_back(std::move(empty_action));
    }

    fsm::state_id_t new_initial_id = new_initial();
    fsm::state_id_t new_final_id = nfa_result->new_state();

    for(const std::unique_ptr<rbg_parser::game_move> & child : move.get_content())
    {
        child->accept(*this);
        (*nfa_result)[new_initial_id].add_transition(nfa_result->initial(), used_actions["epsilon"]);
        (*nfa_result)[nfa_result->final()].add_transition(new_final_id, used_actions["epsilon"]);
        stop_reusing_finals();
    }
    nfa_result->set_initial(new_initial_id);
    nfa_result->set_final(new_final_id);
    last_final = new_final_id;
    start_reusing_finals();
}

void game_nfa_creator::dispatch(const rbg_parser::pure_sum& move) {
    if(used_actions.find("epsilon") == used_actions.end())
    {
        std::unique_ptr<action> empty_action(new actions::empty());
        used_actions["epsilon"] = empty_action.get();
        actions.push_back(std::move(empty_action));
    }

    fsm::state_id_t new_initial_id = new_initial();
    fsm::state_id_t new_final_id = nfa_result->new_state();

    for(const std::unique_ptr<rbg_parser::pure_game_move> & child : move.get_content())
    {
        child->accept(*this);
        (*nfa_result)[new_initial_id].add_transition(nfa_result->initial(), used_actions["epsilon"]);
        (*nfa_result)[nfa_result->final()].add_transition(new_final_id, used_actions["epsilon"]);
        stop_reusing_finals();
    }
    nfa_result->set_initial(new_initial_id);
    nfa_result->set_final(new_final_id);
    last_final = new_final_id;
    start_reusing_finals();
}

void game_nfa_creator::dispatch(const rbg_parser::concatenation& move) {
    move.get_content().front()->accept(*this);

    fsm::state_id_t initial = nfa_result->initial();

    for(size_t i = 1; i < move.get_content().size(); i++)
    {
        const auto& child = move.get_content()[i];
        start_reusing_finals();
        child->accept(*this);
    }
    nfa_result->set_initial(initial);
    last_final = nfa_result->final();
    start_reusing_finals();
}

void game_nfa_creator::dispatch(const rbg_parser::pure_concatenation& move) {
    move.get_content().front()->accept(*this);

    fsm::state_id_t initial = nfa_result->initial();

    for(size_t i = 1; i < move.get_content().size(); i++)
    {
        const auto& child = move.get_content()[i];
        start_reusing_finals();
        child->accept(*this);
    }
    nfa_result->set_initial(initial);
    last_final = nfa_result->final();
    start_reusing_finals();
}

void game_nfa_creator::dispatch(const rbg_parser::bracketed_move& move) {
    if(move.is_star())
    {
        if(used_actions.find("epsilon") == used_actions.end())
        {
            std::unique_ptr<action> empty_action(new actions::empty());
            used_actions["epsilon"] = empty_action.get();
            actions.push_back(std::move(empty_action));
        }

        fsm::state_id_t new_initial_id = new_initial();
        fsm::state_id_t new_final_id = nfa_result->new_state();

        move.get_content()->accept(*this);

        (*nfa_result)[new_initial_id].add_transition(nfa_result->initial(), used_actions["epsilon"] );
        (*nfa_result)[new_initial_id].add_transition(new_final_id, used_actions["epsilon"]);
        (*nfa_result)[nfa_result->final()].add_transition(new_final_id, used_actions["epsilon"]);
        (*nfa_result)[nfa_result->final()].add_transition(nfa_result->initial(), used_actions["epsilon"]);

        nfa_result->set_initial(new_initial_id);
        nfa_result->set_final(new_final_id);
        last_final = new_final_id;
        start_reusing_finals();
        return;
    }

    move.get_content()->accept(*this);

    fsm::state_id_t initial = nfa_result->initial();

    for(size_t i = 1; i < move.get_number_of_repetitions(); i++)
    {
        start_reusing_finals();
        move.get_content()->accept(*this);
    }
    nfa_result->set_initial(initial);
    last_final = nfa_result->final();
    start_reusing_finals();
}

void game_nfa_creator::dispatch(const rbg_parser::pure_bracketed_move& move) {
    if(move.is_star())
    {
        if(used_actions.find("epsilon") == used_actions.end())
        {
            std::unique_ptr<action> empty_action(new actions::empty());
            used_actions["epsilon"] = empty_action.get();
            actions.push_back(std::move(empty_action));
        }

        fsm::state_id_t new_initial_id = new_initial();
        fsm::state_id_t new_final_id = nfa_result->new_state();

        move.get_content()->accept(*this);

        (*nfa_result)[new_initial_id].add_transition(nfa_result->initial(), used_actions["epsilon"] );
        (*nfa_result)[new_initial_id].add_transition(new_final_id, used_actions["epsilon"]);
        (*nfa_result)[nfa_result->final()].add_transition(new_final_id, used_actions["epsilon"]);
        (*nfa_result)[nfa_result->final()].add_transition(nfa_result->initial(), used_actions["epsilon"]);

        nfa_result->set_initial(new_initial_id);
        nfa_result->set_final(new_final_id);
        last_final = new_final_id;
        start_reusing_finals();
        return;
    }

    move.get_content()->accept(*this);

    fsm::state_id_t initial = nfa_result->initial();

    for(size_t i = 1; i < move.get_number_of_repetitions(); i++)
    {
        start_reusing_finals();
        move.get_content()->accept(*this);
    }
    nfa_result->set_initial(initial);
    last_final = nfa_result->final();
    start_reusing_finals();
}

void game_nfa_creator::dispatch(const rbg_parser::shift& move) {
    std::string move_identifier = move.to_rbg();
    if(used_actions.find(move_identifier) == used_actions.end())
    {
        std::unique_ptr<action> action(new actions::shift(move.get_x(),move.get_y()));
        used_actions[move_identifier] = action.get();
        actions.push_back(std::move(action));
    }
    fsm::state_id_t initial_id = new_initial();
    fsm::state_id_t final_id = nfa_result->new_state();
    (*nfa_result)[initial_id].add_transition(final_id, used_actions[move_identifier]);
    nfa_result->set_initial(initial_id);
    nfa_result->set_final(final_id);

    last_final = final_id;
    start_reusing_finals();
}

void game_nfa_creator::dispatch(const rbg_parser::ons& move) {
    std::string move_identifier = move.to_rbg();
    if(used_actions.find(move_identifier) == used_actions.end())
    {
        std::vector<token_id_t> pieces;
        for(const auto& piece_token : move.get_legal_ons())
        {
            pieces.emplace_back(resolver.id(piece_token.to_string()));
        }
        std::unique_ptr<action> action(new actions::on(std::unordered_set<token_id_t>(pieces.begin(),pieces.end())));
        used_actions[move_identifier] = action.get();
        actions.push_back(std::move(action));
    }
    fsm::state_id_t initial_id = new_initial();
    fsm::state_id_t final_id = nfa_result->new_state();
    (*nfa_result)[initial_id].add_transition(final_id, used_actions[move_identifier]);
    nfa_result->set_initial(initial_id);
    nfa_result->set_final(final_id);

    last_final = final_id;
    start_reusing_finals();
}

void game_nfa_creator::dispatch(const rbg_parser::off& move) {
    fsm::state_id_t initial_id = new_initial();
    fsm::state_id_t final_id = nfa_result->new_state();
    register_modifier(initial_id);
    token_id_t piece_id = resolver.id(move.get_piece().to_string());
    std::unique_ptr<action> letter(new actions::off((unsigned int) blocks_states.size() - 1, piece_id));
    action* letter_ptr = letter.get();
    actions.push_back(std::move(letter));
    if(move.is_lazy())
    {
        std::unique_ptr<action> lazy_letter(new actions::lazy((unsigned int) blocks_states.size() - 1, letter_ptr));
        letter_ptr = lazy_letter.get();
        actions.push_back(std::move(lazy_letter));
    }
    (*nfa_result)[initial_id].add_transition(final_id, letter_ptr);
    nfa_result->set_initial(initial_id);
    nfa_result->set_final(final_id);

    last_final = final_id;
    start_reusing_finals();
}

void game_nfa_creator::dispatch(const rbg_parser::assignment& move) {
    fsm::state_id_t initial_id = new_initial();
    fsm::state_id_t final_id = nfa_result->new_state();
    register_modifier(initial_id);
    token_id_t variable_id = resolver.id(move.get_left_side().to_string());
    int value = move.get_right_side().get_value();
    std::unique_ptr<action> letter(new actions::assignment((unsigned int) blocks_states.size() - 1, variable_id, value));
    action* letter_ptr = letter.get();
    actions.push_back(std::move(letter));
    if(move.is_lazy())
    {
        std::unique_ptr<action> lazy_letter(new actions::lazy((unsigned int) blocks_states.size() - 1, letter_ptr));
        letter_ptr = lazy_letter.get();
        actions.push_back(std::move(lazy_letter));
    }
    (*nfa_result)[initial_id].add_transition(final_id, letter_ptr);
    nfa_result->set_initial(initial_id);
    nfa_result->set_final(final_id);

    last_final = final_id;
    start_reusing_finals();
}

void game_nfa_creator::dispatch(const rbg_parser::player_switch& move) {
    fsm::state_id_t initial_id = new_initial();
    fsm::state_id_t final_id = nfa_result->new_state();
    register_modifier(initial_id);
    std::unique_ptr<action> letter;
    if(move.changes_player()) {
        token_id_t player_id = resolver.id(move.get_player().to_string());
        letter = std::unique_ptr<action>(new actions::player_switch((unsigned int) blocks_states.size() - 1, player_id));
    }
    else
    {
        letter = std::unique_ptr<action>(new actions::semi_switch((unsigned int) blocks_states.size() - 1));
    }
    action* letter_ptr = letter.get();
    actions.push_back(std::move(letter));
    (*nfa_result)[initial_id].add_transition(final_id, letter_ptr);
    nfa_result->set_initial(initial_id);
    nfa_result->set_final(final_id);

    last_final = final_id;
    start_reusing_finals();
}

void game_nfa_creator::register_modifier(fsm::state_id_t initial_id) {
    if(!block_started && register_modifiers)
        blocks_states.push_back(initial_id);
}

void game_nfa_creator::dispatch(const rbg_parser::condition_check& move) {
    std::string move_identifier = move.to_rbg();
    if(used_actions.find(move_identifier) == used_actions.end())
    {
        move.get_content()->accept(*this);
        std::unique_ptr<action> action(new actions::condition_action(get_condition()));
        used_actions[move_identifier] = action.get();
        actions.push_back(std::move(action));
    }
    fsm::state_id_t initial_id = new_initial();
    fsm::state_id_t final_id = nfa_result->new_state();
    (*nfa_result)[initial_id].add_transition(final_id, used_actions[move_identifier]);
    nfa_result->set_initial(initial_id);
    nfa_result->set_final(final_id);

    last_final = final_id;
    start_reusing_finals();
}

void game_nfa_creator::dispatch(const rbg_parser::modifier_block& move) {

    move.get_content().front()->accept(*this);

    fsm::state_id_t initial = nfa_result->initial();
    start_block();
    for(size_t i = 1; i < move.get_content().size(); i++)
    {
        const auto& child = move.get_content()[i];
        start_reusing_finals();
        child->accept(*this);
    }
    stop_block();
    nfa_result->set_initial(initial);
    last_final = nfa_result->final();
    start_reusing_finals();
}

void game_nfa_creator::stop_block() { block_started = false; }
void game_nfa_creator::start_block() { block_started = true; }