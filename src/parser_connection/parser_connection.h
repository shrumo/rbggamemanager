//
// Created by shrum on 13.01.18.
//

#ifndef RBGGAMEMANAGER_PARSER_CONNECTION_H
#define RBGGAMEMANAGER_PARSER_CONNECTION_H

#include "../../rbgParser/src/game_board.hpp"
#include "../../rbgParser/src/declarations.hpp"
#include "../../rbgParser/src/game_move.hpp"
#include "../../rbgParser/src/parsed_game.hpp"

class board;
class name_resolver;
class game_moves_description;
class game_description;
class declarations_description;

name_resolver create_resolver(const rbg_parser::declarations& declarations);
board create_board(const rbg_parser::game_board& parsed_board, const name_resolver& resolver);
declarations_description create_declarations(const rbg_parser::declarations& declarations, const name_resolver& resolver);
game_moves_description create_moves(const rbg_parser::game_move& move, const name_resolver& resolver);
game_description  create_description(const rbg_parser::parsed_game& game);

#endif //RBGGAMEMANAGER_PARSER_CONNECTION_H
