//
// Created by shrum on 07.11.20.
//

#ifndef RBGGAMEMANAGER_SHIFT_TABLE_CHECKER_H
#define RBGGAMEMANAGER_SHIFT_TABLE_CHECKER_H

#include <parser/parser_actions.h>
#include <parser/parser_wrapper.h>

namespace rbg {
bool ContainsOnlyShifts(const rbg_parser::game_move &move);
}

#endif // RBGGAMEMANAGER_PRINTER_H
