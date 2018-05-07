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

// TODO(shrum): Change this to graph representation.
class Board {
public:
  Board(size_t width, size_t height, token_id_t initial_value = 0)
      : fields_(height * width, initial_value),
        width_(width),
        height_(height) {}

  size_t width() const { return width_; }

  size_t height() const { return height_; }

  token_id_t &operator()(size_t x, size_t y) {
    return fields_[y * width_ + x];
  }

  token_id_t operator()(size_t x, size_t y) const {
    return fields_[y * width_ + x];
  }

private:
  std::vector<token_id_t> fields_;
  size_t width_;
  size_t height_;
};


#endif //RBGGAMEMANAGER_BOARD_H
