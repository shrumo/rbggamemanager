#ifndef RBGGAMEMANAGER__UTILITY_ILLEGAL_DOTS_H
#define RBGGAMEMANAGER__UTILITY_ILLEGAL_DOTS_H

#include "game_state/construction/game_state_creator.h"

namespace rbg {

// We define an illegal dot as a dot in the rules that would increase the number
// of moves if it was treated as a modifier. This locates such illegal dots
// in the nfa. (the game_moves are pointers coming from original_move annotation
// on the edges of the Nfa)
// Example usage:
//    Declarations declarations = Declarations(*parsed_game);
//    auto nfa =
//        CreateNfa(*parsed_game->get_moves(), declarations, /*optimize=*/false);
//    unordered_set<const rbg_parser::game_move *> illegal_noop_moves =
//        FindIllegalNoops(nfa, declarations);
std::unordered_set<const rbg_parser::game_move *>
FindIllegalNoops(const Nfa<std::unique_ptr<Move>> &nfa,
                 const Declarations &declarations);
} // namespace rbg

#endif // RBGGAMEMANAGER__UTILITY_ILLEGAL_DOTS_H