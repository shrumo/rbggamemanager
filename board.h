//
// Created by shrum on 02.01.18.
//

#ifndef RBGGAMEMANAGER_BOARD_H
#define RBGGAMEMANAGER_BOARD_H

#include <vector>
#include "rbg2gdl/src/game_board.hpp"
#include "name_resolver.h"

class board
{
    std::vector<std::vector<token_id_t> > fields;
public:
    board(const rbg_parser::game_board& parsed_board,const name_resolver& resolver)
            :fields(parsed_board.get_height(),std::vector<token_id_t>(parsed_board.get_width()))
    {
        for(std::vector<std::vector<token_id_t>>::size_type y = 0; y < height(); y++)
        {
            for(std::vector<token_id_t >::size_type x = 0; x < width(); x++)
            {
                std::string name = parsed_board.get_square((uint) x, (uint) y).to_string();
                fields[y][x] = resolver.get_piece_id(name);
            }
        }
    }

    std::vector<token_id_t>::size_type width() const
    {
        return (int) fields[0].size();
    }
    std::vector<std::vector<token_id_t>>::size_type height() const
    {
        return (int) fields.size();
    }

    token_id_t & operator()(std::vector<token_id_t>::size_type x,std::vector<std::vector<token_id_t>>::size_type y)
    {
        return fields[y][x];
    }

    token_id_t operator()(std::vector<token_id_t>::size_type x,std::vector<std::vector<token_id_t>>::size_type y) const
    {
        return fields[y][x];
    }
};


#endif //RBGGAMEMANAGER_BOARD_H
