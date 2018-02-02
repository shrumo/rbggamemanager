//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_GAME_DESCRIPTION_H
#define RBGGAMEMANAGER_GAME_DESCRIPTION_H


#include "name_resolver.h"
#include "../game_nfa/automaton.h"
#include "../game_nfa/game_moves_description.h"
#include "board.h"
#include "../../rbgParser/src/parsed_game.hpp"

class declarations_description
{
    std::vector<token_id_t> players_ids;
    std::vector<token_id_t> variables_ids;
    std::vector<token_id_t> pieces_ids;
public:
    declarations_description() = default;

    void add_player(token_id_t player_id)
    {
        players_ids.push_back(player_id);
    }

    void add_variable(token_id_t variable_id)
    {
        variables_ids.push_back(variable_id);
    }

    void add_piece(token_id_t piece_id)
    {
        pieces_ids.push_back(piece_id);
    }

    const std::vector<token_id_t> &get_players_ids() const {
        return players_ids;
    }

    const std::vector<token_id_t> &get_variables_ids() const {
        return variables_ids;
    }

    const std::vector<token_id_t> &get_pieces_ids() const {
        return pieces_ids;
    }
};

class game_description {
    name_resolver resolver;
    game_moves_description moves_description;
    token_id_t turn_variable_id;
    board initial_board;
    token_id_t deterministic_keeper_player_id;
    token_id_t nondeterministic_keeper_player_id;

    declarations_description declarations;

    std::string game_text;

public:
    game_description(name_resolver name_resolver, game_moves_description moves_description,
                     board initial_board, declarations_description declarations, std::string game_description)
            : resolver(std::move(name_resolver)), moves_description(std::move(moves_description)),
              turn_variable_id(resolver.id("turn")), /* TODO(shrum): Ask what about the turn variable */
              initial_board(std::move(initial_board)),
              deterministic_keeper_player_id(0),
              nondeterministic_keeper_player_id(0),
              declarations(std::move(declarations)),
              game_text(game_description)
    {
        resolver.add_name("epsilon");
        deterministic_keeper_player_id = resolver.id("epsilon");

        resolver.add_name("*");
        nondeterministic_keeper_player_id = resolver.id("*");
    }

    const std::string get_text_description() const
    {
        return game_text;
    }

    const declarations_description& get_declarations() const
    {
        return declarations;
    }

    const name_resolver& get_resolver() const {
        return resolver;
    }

    const game_moves_description& get_moves_description() const {
        return moves_description;
    }

    token_id_t get_turn_id() const {
        return turn_variable_id;
    }

    const board& get_initial_board() const {
        return initial_board;
    }

    size_t get_variables_count() const
    {
        return resolver.names_count();
    }

    token_id_t get_deterministic_keeper_player_id() const {
        return deterministic_keeper_player_id;
    }

    token_id_t get_nondeterministic_keeper_player_id() const {
        return nondeterministic_keeper_player_id;
    }
};




#endif //RBGGAMEMANAGER_GAME_DESCRIPTION_H
