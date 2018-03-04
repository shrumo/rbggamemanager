//
// Created by shrum on 13.01.18.
//

#ifndef RBGGAMEMANAGER_PARSER_CONNECTION_H
#define RBGGAMEMANAGER_PARSER_CONNECTION_H

#include "../../rbgParser/src/game_board.hpp"
#include "../../rbgParser/src/declarations.hpp"
#include "../../rbgParser/src/game_move.hpp"
#include "../../rbgParser/src/parsed_game.hpp"

class Board;

class NameResolver;

class GameMovesDescription;

class GameDescription;

class Declarations;

NameResolver CreateResolver(const rbg_parser::declarations &declarations);

Board CreateBoard(const rbg_parser::game_board &parsed_board,
                  const NameResolver &resolver);

Declarations CreateDeclarations(const rbg_parser::declarations &declarations,
                                const NameResolver &resolver);

GameMovesDescription
CreateMoves(const rbg_parser::game_move &move, const NameResolver &resolver);

GameDescription CreateDescription(const rbg_parser::parsed_game &game);

#endif //RBGGAMEMANAGER_PARSER_CONNECTION_H
