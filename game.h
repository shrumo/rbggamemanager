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

    name_resolver(const parsed_game& parsed_game)
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
    board(const game_board& parsed_board,const name_resolver& resolver)
        :fields(parsed_board.get_height(),std::vector<int>(parsed_board.get_width()))
    {
        for(int y = 0; y < height(); y++)
        {
            for(int x = 0; x < width(); x++)
            {
                fields[y][x] = resolver.pieces_ids.at(parsed_board.get_square((uint) x, (uint) y).to_string());
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

    game_description(const parsed_game& parsed_game)
        : resolver(parsed_game), state_register(new fsm::state_register()), game_nfa(state_register.get())
    {
        game_nfa_creator creator(state_register.get(),resolver);
        parsed_game.get_moves()->accept(creator);
        game_nfa = creator.get_result();
    }
};

class game_state
{
public:
//    game& parent;
//    board board;
    int board_x;
    int board_y;
    int index;
    std::vector<int> sigma;
    int current_player;
    int turn;

};


#endif //RBGGAMEMANAGER_GAME_H
