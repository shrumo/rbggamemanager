#ifndef RBGGAMEMANAGER__UTILITY_ILLEGAL_DOTS_H
#define RBGGAMEMANAGER__UTILITY_ILLEGAL_DOTS_H

#include "game_state/construction/game_state_creator.h"

namespace rbg {
std::unordered_set<const rbg_parser::game_move *>
FindIllegalNoops(const Nfa<std::unique_ptr<Move>> &nfa,
                 const Declarations &declarations);
}

#endif // RBGGAMEMANAGER__UTILITY_ILLEGAL_DOTS_H