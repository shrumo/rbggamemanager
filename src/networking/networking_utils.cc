//
// Created by shrum on 01/02/18.
//

#include <iostream>
#include "networking_utils.h"

#include "message.h"


std::string MoveDataString(const Move &move) {
  std::stringstream stream;
  for (size_t i = 0; i < move.blocks().size(); i++) {
    const auto &block = move.blocks()[i];
    stream << block.x() << " " << block.y() << " " << block.id();
    if (i != move.blocks().size() - 1)
      stream << " ";
  }
  return stream.str();
}

Move DecodeMove(const Message &message) {
  std::stringstream stream(std::string(message.body_ptr(), message.body_ptr() +
                                                           message.body_length()));
  Move result;
  while (stream.peek() != std::char_traits<char>::eof()) {
    size_t block_x, block_y, block_id;
    stream >> block_x >> block_y >> block_id >> std::ws;
    result.AddBlock(block_x, block_y, block_id);
  }
  return result;
}
