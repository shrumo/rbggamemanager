//
// Created by shrum on 16.06.19.
//

#ifndef RBGGAMEMANAGER_DECLARATIONS_CREATOR_H
#define RBGGAMEMANAGER_DECLARATIONS_CREATOR_H

#include <game_state/declarations.h>

namespace rbg {
  Declarations CreateDeclarations(const rbg_parser::parsed_game &game);
}

#endif //RBGGAMEMANAGER_DECLARATIONS_CREATOR_H
