//
// Created by shrum on 17.06.19.
//

#ifndef RBGGAMEMANAGER_MOVES_PRINTER_H
#define RBGGAMEMANAGER_MOVES_PRINTER_H

#include <string>
#include <game_state/declarations.h>
#include "moves.h"

namespace rbg {
  std::string MoveDescription(const Move &move, const Declarations &declarations);
  std::unordered_map<uint,std::string> ActionsDescriptionsMap(const std::string& game_text);
}


#endif //RBGGAMEMANAGER_MOVES_PRINTER_H
