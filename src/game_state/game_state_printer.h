//
// Created by shrum on 30.06.19.
//

#ifndef RBGGAMEMANAGER_GAME_STATE_PRINTER_H
#define RBGGAMEMANAGER_GAME_STATE_PRINTER_H

#include <string>
#include <sstream>
#include <cassert>
#include <iomanip>
#include "board.h"
#include "declarations.h"

namespace rbg {
  std::string RectangularBoardDescription(const BoardContent &board_content, const Declarations &declarations);
}
#endif //RBGGAMEMANAGER_GAME_STATE_PRINTER_H
