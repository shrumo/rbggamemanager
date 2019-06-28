//
// Created by shrum on 11.06.19.
//

#ifndef RBGGAMEMANAGER_DECLARATIONS_H
#define RBGGAMEMANAGER_DECLARATIONS_H

#include <rbgParser/src/parsed_game.hpp>
#include "namesresolver.h"
#include "board.h"

namespace rbg {
  using variable_id_t = name_id_t;
  using player_id_t = name_id_t;
  using variable_value_t = uint;

  struct Declarations {
    NamesResolver pieces_resolver;
    NamesResolver variables_resolver;
    NamesResolver players_resolver;
    std::vector<variable_value_t> variables_bounds;
    Board board_description;
    player_id_t keeper_id;
  };
}

#endif //RBGGAMEMANAGER_DECLARATIONS_H
