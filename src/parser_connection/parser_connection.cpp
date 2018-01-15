//
// Created by shrum on 13.01.18.
//

#include "parser_connection.h"
#include "../game_components/board.h"
#include "../game_components/game_description.h"

board create_board(const rbg_parser::game_board& parsed_board, const name_resolver& resolver)
{
    board result(parsed_board.get_width(), parsed_board.get_height());
    for(size_t y = 0; y < result.height(); y++)
    {
        for(size_t x = 0; x < result.width(); x++)
        {
            std::string name = parsed_board.get_square((uint) x, (uint) y).to_string();
            result(x,y) = resolver.id(name);
        }
    }
    return result;
}

name_resolver create_resolver(const rbg_parser::declarations& declarations)
{
    name_resolver result;
    for(const auto& piece_token : declarations.get_legal_pieces())
    {
        std::string piece_name = piece_token.to_string();
        result.add_name(piece_name);
    }
    for(const auto& player_token : declarations.get_legal_players())
    {
        std::string player_name = player_token.to_string();
        result.add_name(player_name);
    }
    for(const auto& variable_token : declarations.get_legal_variables())
    {
        std::string variable_name = variable_token.to_string();
        result.add_name(variable_name);
    }
    result.add_name("turn");
    return result;
}


game_moves_description create_moves(const rbg_parser::game_move& move, const name_resolver &resolver) {
    game_nfa_creator creator(resolver);
    move.accept(creator);
    return creator.create_description();
}


game_description create_description(const rbg_parser::parsed_game &game) {
    name_resolver resolver(create_resolver(game.get_declarations()));
    board initial_board(create_board(game.get_board(),resolver));
    game_moves_description moves(create_moves(*game.get_moves(),resolver));
    token_id_t first_player_id = resolver.id(game.get_declarations().get_first_player().to_string());
    return std::move(game_description(std::move(resolver), std::move(moves), std::move(initial_board), first_player_id));
}
