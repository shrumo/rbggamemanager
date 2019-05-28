//
// Created by shrum on 28.05.19.
//

#ifndef RBGGAMEMANAGER_GRAPH_CREATOR_H
#define RBGGAMEMANAGER_GRAPH_CREATOR_H

#include <game_nfa/graph.h>
#include <memory>
#include <actions/actions.h>
#include <rbgParser/src/game_move.hpp>

namespace rbg {
  Nfa<std::unique_ptr<Action>> CreateGraph(const rbg_parser::game_move &move);
}


#endif //RBGGAMEMANAGER_GRAPH_CREATOR_H
