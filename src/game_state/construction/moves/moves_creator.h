//
// Created by shrum on 16.06.19.
//

#ifndef RBGGAMEMANAGER_MOVES_CREATOR_H
#define RBGGAMEMANAGER_MOVES_CREATOR_H

#include "moves.h"

namespace rbg {
  std::unique_ptr<Move> CreateMove(const rbg_parser::game_move &move, const Declarations &declarations);
}

#endif //RBGGAMEMANAGER_MOVES_CREATOR_H
