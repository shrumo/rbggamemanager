//
// Created by shrum on 28.05.19.
//

#ifndef RBGGAMEMANAGER_GRAPH_CREATOR_H
#define RBGGAMEMANAGER_GRAPH_CREATOR_H

#include <graph/graph.h>
#include <memory>
#include <rbgParser/src/game_move.hpp>
#include "construction/moves/moves.h"

namespace rbg {
  struct NfaWithVisitedChecks {
    uint visited_checks_count;
    Nfa<std::unique_ptr<Move>> nfa;
  };

  NfaWithVisitedChecks CreateGraph(const rbg_parser::game_move &move, const Declarations &declarations);
}


#endif //RBGGAMEMANAGER_GRAPH_CREATOR_H
