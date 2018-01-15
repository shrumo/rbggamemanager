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

class game_description {
    name_resolver resolver;
    game_moves_description moves_description;
    token_id_t turn_variable_id;
    board initial_board;
    token_id_t starting_player;
    size_t variables_count;
public:
    game_description(name_resolver name_resolver, game_moves_description moves_description,
                     board initial_board, token_id_t starting_player) : resolver(
            std::move(name_resolver)), moves_description(std::move(moves_description)), initial_board(
            std::move(initial_board)), starting_player(starting_player), variables_count(resolver.names_count())
    {
        turn_variable_id = resolver.id("turn");
    }

    game_description(size_t width, size_t height)
        : resolver(), moves_description(), turn_variable_id(0), initial_board(width, height), starting_player(0)
        , variables_count(1)
    {
        resolver.add_name("turn");
        turn_variable_id = resolver.id("turn");
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

    token_id_t get_starting_player() const {
        return starting_player;
    }

    size_t get_variables_count() const
    {
        return variables_count;
    }
};



#endif //RBGGAMEMANAGER_GAME_DESCRIPTION_H
