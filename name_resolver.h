//
// Created by shrum on 02.01.18.
//

#ifndef RBGGAMEMANAGER_NAME_RESOLVER_H
#define RBGGAMEMANAGER_NAME_RESOLVER_H

#include <unordered_map>
#include "rbg2gdl/src/parsed_game.hpp"

using token_id_t = std::vector<std::string>::size_type;

class name_resolver
{
    std::vector<std::string> players;
    std::vector<std::string> pieces;
    std::vector<std::string> variables;

    std::unordered_map<std::string,token_id_t> players_ids;
    std::unordered_map<std::string,token_id_t> pieces_ids;
    std::unordered_map<std::string,token_id_t> variables_ids;

    void add_player_name(const std::string& player_name)
    {
        players.push_back(player_name);
        players_ids[player_name] =  players.size() - 1;
    }

    void add_variable_name(const std::string& variable_name)
    {
        variables.push_back(variable_name);
        variables_ids[variable_name] = variables.size() - 1;
    }

    void add_piece_name(const std::string& piece_name)
    {
        pieces.push_back(piece_name);
        pieces_ids[piece_name] = pieces.size() - 1;
    }

public:
    name_resolver(const rbg_parser::parsed_game& parsed_game)
    {
        for(const auto& piece_token : parsed_game.get_declarations().get_legal_pieces())
        {
            std::string piece_name = piece_token.to_string();
            add_piece_name(piece_name);
            add_variable_name(piece_name);
        }
        for(const auto& player_token : parsed_game.get_declarations().get_legal_players())
        {
            std::string player_name = player_token.to_string();
            add_player_name(player_name);
            add_variable_name(player_name);
        }
        for(const auto& variable_token : parsed_game.get_declarations().get_legal_variables())
        {
            std::string variable_name = variable_token.to_string();
            add_variable_name(variable_name);
        }

        add_variable_name("turn");
    }

    token_id_t get_player_id(const std::string& player_name) const
    {
        return players_ids.at(player_name);
    }

    token_id_t get_piece_id(const std::string& piece_name) const
    {
        return pieces_ids.at(piece_name);
    }

    token_id_t get_variable_id(const std::string& variable_name) const
    {
        return variables_ids.at(variable_name);
    }

    const std::string& get_player_name(token_id_t player_id) const
    {
        return players[player_id];
    }

    const std::string& get_piece_name(token_id_t piece_id) const
    {
        return pieces[piece_id];
    }

    size_t variables_count() const
    {
        return variables.size();
    }

    size_t players_count() const
    {
        return players.size();
    }
};



#endif //RBGGAMEMANAGER_NAME_RESOLVER_H
