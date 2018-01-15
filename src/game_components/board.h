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
    std::vector<std::vector<token_id_t> > fields;
public:
    board(const rbg_parser::game_board& parsed_board, const name_resolver& resolver)
            :fields(parsed_board.get_height(),std::vector<token_id_t>(parsed_board.get_width()))
    {
        for(size_t y = 0; y < height(); y++)
        {
            for(size_t x = 0; x < width(); x++)
            {
                std::string name = parsed_board.get_square((uint) x, (uint) y).to_string();
                fields[y][x] = resolver.id(name);
            }
        }
    }

    board(size_t width, size_t height, token_id_t initial_value=0)
            : fields(height, std::vector<token_id_t>(width, initial_value))
    {}

    size_t width() const
    {
        return fields[0].size();
    }
    size_t height() const
    {
        return fields.size();
    }

    token_id_t& operator()(size_t x,size_t y)
    {
        return fields[y][x];
    }

    token_id_t operator()(size_t x,size_t y) const
    {
        return fields[y][x];
    }
};



#endif //RBGGAMEMANAGER_BOARD_H
