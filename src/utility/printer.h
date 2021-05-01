//
// Created by shrum on 17.06.19.
//

#ifndef RBGGAMEMANAGER_PRINTER_H
#define RBGGAMEMANAGER_PRINTER_H

#include "game_description/moves/moves.h"
#include <game_description/declarations.h>
#include <string>

namespace rbg {
std::string MoveDescription(const Move &move, const Declarations &declarations);
std::unordered_map<uint, std::string>
ActionsDescriptionsMap(const std::string &game_text);

std::string RectangularBoardDescription(const BoardContent &board_content,
                                        const Declarations &declarations);
std::string VariablesValuesDescription(const GameState &state);
} // namespace rbg

#endif // RBGGAMEMANAGER_PRINTER_H
