#ifndef RBGGAMEMANAGER__UTILITY_REDUNDANT_DOTS_H
#define RBGGAMEMANAGER__UTILITY_REDUNDANT_DOTS_H

#include "game_state/construction/game_state_creator.h"

namespace rbg {

enum class NoopDeterminedState {
    NON_DETERMINED = 0,
    LEFT_DETERMINED = 1,
    RIGHT_DETERMINED = 2,
    FULLY_DETERMINED = 3
};

// We define a redundant dot as a dot whose application is fully determined by
// an application of a modifier or some other dot before or after it. There are
// two kinds of redundant dots: front redundat (which are determined by a
// modifier/dot to the front of the expression) and back redundant. This
// locates function located such redundant dots. Example usage:
//    Declarations declarations = Declarations(*parsed_game);
//    auto nfa =
//        CreateNfa(*parsed_game->get_moves(), declarations,
//        /*optimize=*/false);
//    unordered_set<const rbg_parser::game_move *> redundant_noop_moves =
//        FindRedundantNoops(nfa, declarations);
std::unordered_map<const rbg_parser::game_move *, NoopDeterminedState>
FindRedundantNoops(const Nfa<std::unique_ptr<Move>> &nfa,
                    const Declarations &declarations);
} // namespace rbg

#endif // RBGGAMEMANAGER__UTILITY_REDUNDANT_DOTS_H