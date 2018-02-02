//
// Created by shrum on 01/02/18.
//

#include <iostream>
#include "networking_utils.h"

#include "message.h"


std::string move_data_string(const move &move) {
    std::stringstream stream;
    for(size_t i = 0; i < move.get_blocks().size(); i++)
    {
        const auto& block = move.get_blocks()[i];
        stream << block.x() <<" "<< block.y() << " " << block.get_block_id();
        if(i != move.get_blocks().size()-1)
            stream << " ";
    }
    return stream.str();
}

move decode_move(const message &message) {
    std::stringstream stream(std::string(message.body_ptr(),message.body_ptr() + message.get_body_length()));
    move result;
    while(stream.peek() != std::char_traits<char>::eof())
    {
        size_t block_x, block_y, block_id;
        stream >> block_x >> block_y >> block_id >> std::ws;
        result.add_block(block_x,block_y,block_id);
    }
    return result;
}
