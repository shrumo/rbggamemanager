//
// Created by shrum on 28.12.17.
//

#ifndef RBGGAMEMANAGER_GAME_H
#define RBGGAMEMANAGER_GAME_H

#include "rbg2gdl/src/token.hpp"
#include "automaton.h"
#include "rbg2gdl/src/parsed_game.hpp"
#include <unordered_map>

class name_resolver
{
public:
    std::vector<std::string> players;
    std::vector<std::string> pieces;
    std::vector<std::string> variables;

    std::unordered_map<std::string,int> players_ids;
    std::unordered_map<std::string,int> pieces_ids;
    std::unordered_map<std::string,int> variables_ids;

    name_resolver(const rbg_parser::parsed_game& parsed_game)
    {
        for(const auto& player_token : parsed_game.get_declarations().get_legal_players())
        {
            std::string player_name = player_token.to_string();
            players.push_back(player_name);
            players_ids[player_name] = (int) (players.size() - 1);
            variables.push_back(player_name);
            variables_ids[player_name] = (int) (variables.size() - 1);
        }
        for(const auto& piece_token : parsed_game.get_declarations().get_legal_pieces())
        {
            std::string piece_name = piece_token.to_string();
            pieces.push_back(piece_name);
            pieces_ids[piece_name] = (int) (pieces.size() - 1);
        }
        for(const auto& variable_token : parsed_game.get_declarations().get_legal_variables())
        {
            std::string variable_name = variable_token.to_string();
            variables.push_back(variable_name);
            variables_ids[variable_name] = (int) (variables.size() - 1);
        }
    }
};

class board
{
public:
    std::vector<std::vector<int>> fields;
    board(const rbg_parser::game_board& parsed_board,const name_resolver& resolver)
        :fields(parsed_board.get_height(),std::vector<int>(parsed_board.get_width()))
    {
        for(int y = 0; y < height(); y++)
        {
            for(int x = 0; x < width(); x++)
            {
                std::string name = parsed_board.get_square((uint) x, (uint) y).to_string();
                fields[y][x] = resolver.pieces_ids.at(name);
            }
        }
    }

    int width()
    {
        return (int) fields[0].size();
    }
    int height()
    {
        return (int) fields.size();
    }
};

class game_description {
public:
    name_resolver resolver;
    std::unique_ptr<fsm::state_register> state_register;
    fsm::nfa game_nfa;
    board initial_board;
    int starting_player_id;

    game_description(const rbg_parser::parsed_game& parsed_game)
        : resolver(parsed_game), state_register(new fsm::state_register()),
          game_nfa(state_register.get()), initial_board(parsed_game.get_board(),resolver),
          starting_player_id(0)
    {
        game_nfa_creator creator(state_register.get(),resolver);
        parsed_game.get_moves()->accept(creator);
        game_nfa = creator.get_result();
    }
};

class game_state
{
public:
    const game_description& parent;
    board current_board;
    int board_x;
    int board_y;
    int index;
    std::vector<int> sigma;
    int current_player;
    int turn;

    int current_nfa_state;

    std::vector<std::unique_ptr<action> > lazy_actions;
    int lazy_ptr;

    game_state(const game_description& parent)
            : parent(parent),
              current_board(parent.initial_board),
              board_x(0),
              board_y(0),
              index(0),
              sigma(parent.resolver.variables.size()),
              current_player(0),
              turn(0),
              lazy_actions(),
              lazy_ptr(0),
              current_nfa_state(parent.game_nfa.get_initial_id())
    {}

    int current_piece() const
    {
        return current_board.fields[board_y][board_x];
    }

    void set_piece(int piece_id)
    {
        current_board.fields[board_y][board_x] = piece_id;
    }

    int get_lazy_ptr() const
    {
        return lazy_ptr;
    }

    void evaluate_lazy()
    {
        while(lazy_ptr < lazy_actions.size())
        {
            lazy_actions[lazy_ptr]->apply(this);
            lazy_ptr++;
        }
    }

    void revert_lazy(int ptr)
    {
        while(lazy_ptr > ptr)
        {
            lazy_actions[lazy_ptr-1]->revert(this);
            lazy_ptr--;
        }
    }

    long long small_identifier()
    {
        return (long long) (((long long)board_y * current_board.width() + board_x) * parent.game_nfa.get_state_register()->states.size() + current_nfa_state);
    }
};

class block_action
{
    int first_modifier_index;
    int board_position_x;
    int board_position_y;
};

class move
{
    std::vector<block_action> move_actions;
};

std::vector<move> find_all_moves(game_state current);

void find_all_moves_rec(game_state* state,move* current_move, std::vector<move>* result, const std::unordered_set<long long> visited);


#endif //RBGGAMEMANAGER_GAME_H
