//
// Created by shrum on 11.06.19.
//

#ifndef RBGGAMEMANAGER_DECLARATIONS_H
#define RBGGAMEMANAGER_DECLARATIONS_H

#include "namesresolver.h"

namespace rbg {
  using piece_id_t = name_id_t;
  using variable_id_t = name_id_t;
  using player_id_t = name_id_t;

  struct Declarations {
    NamesResolver pieces_resolver;
    NamesResolver variables_resolver;
    NamesResolver players_resolver;
  };
}

#endif //RBGGAMEMANAGER_DECLARATIONS_H
