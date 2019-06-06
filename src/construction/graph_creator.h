//
// Created by shrum on 28.05.19.
//

#ifndef RBGGAMEMANAGER_GRAPH_CREATOR_H
#define RBGGAMEMANAGER_GRAPH_CREATOR_H

#include <graph/graph.h>
#include <memory>
#include <rbgParser/src/game_move.hpp>

namespace rbg {
  using Move = rbg_parser::game_move;

  Nfa<std::unique_ptr<Move>> CreateGraph(const rbg_parser::game_move &move);
}


#endif //RBGGAMEMANAGER_GRAPH_CREATOR_H
