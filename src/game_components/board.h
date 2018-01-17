//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_BOARD_H
#define RBGGAMEMANAGER_BOARD_H

#include <vector>
#include "name_resolver.h"
#include "../../rbgParser/src/game_board.hpp"
#include "../game_nfa/game_moves_description.h"
#include "../../rbgParser/src/game_move.hpp"
#include "../parser_connection/game_nfa_creator.h"

class board
{
    std::vector<token_id_t> fields;
    size_t board_width;
    size_t board_height;
public:
    board(size_t width, size_t height, token_id_t initial_value=0)
            : fields(height * width, initial_value), board_width(width), board_height(height)
    {}

    size_t width() const
    {
        return board_width;
    }
    size_t height() const
    {
        return board_height;
    }

    token_id_t& operator()(size_t x, size_t y)
    {
        return fields[y * board_width + x];
    }

    token_id_t operator()(size_t x,size_t y) const
    {
        return fields[y * board_width + x];
    }
};



#endif //RBGGAMEMANAGER_BOARD_H
