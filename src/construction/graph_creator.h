//
// Created by shrum on 28.05.19.
//

#ifndef RBGGAMEMANAGER_GRAPH_CREATOR_H
#define RBGGAMEMANAGER_GRAPH_CREATOR_H

#include <graph/graph.h>
#include <memory>
#include <utility>
#include <rbgParser/src/game_move.hpp>
#include "construction/moves/moves.h"

namespace rbg {
  struct NodeVertexPair {
    node_t node;
    vertex_id_t vertex;

    bool operator==(const NodeVertexPair &b) const {
      return node == b.node && vertex == b.vertex;
    }
  };
}

namespace std {
  template<>
  struct hash<rbg::NodeVertexPair> {
    std::size_t operator()(const rbg::NodeVertexPair &node_vertex_pair) const {
      return hash<rbg::node_t>()(node_vertex_pair.node) ^ hash<rbg::vertex_id_t>()(node_vertex_pair.vertex);
    }
  };
}


namespace rbg {
  struct NfaWithVisitedChecks {
    uint visited_checks_count;
    Nfa<std::unique_ptr<Move>> nfa;
  };

  struct GraphTimesBoard {
    explicit GraphTimesBoard(const Nfa<std::unique_ptr<Move>> &nfa) : original_nfa(nfa) {}

    const Nfa<std::unique_ptr<Move>> &original_nfa;
    Graph<const Move *> graph_with_board{};
    std::unordered_map<NodeVertexPair, node_t> node_mapping{};
    std::unordered_map<node_t, NodeVertexPair> reverse_node_mapping{};
  };

  GraphTimesBoard CreateGraphTimesBoard(const Nfa<std::unique_ptr<Move>> &nfa, const Declarations &declarations);

  Nfa<std::unique_ptr<Move>> CreateNfa(const rbg_parser::game_move &rbg_move, const Declarations &declarations);

  NfaWithVisitedChecks CreateFinalNfa(const rbg_parser::game_move &rbg_move, const Declarations &declarations);
}



#endif //RBGGAMEMANAGER_GRAPH_CREATOR_H
