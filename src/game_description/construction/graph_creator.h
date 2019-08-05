//
// Created by shrum on 28.05.19.
//

#ifndef RBGGAMEMANAGER_GRAPH_CREATOR_H
#define RBGGAMEMANAGER_GRAPH_CREATOR_H

#include <utility/graph.h>
#include <memory>
#include <utility>
#include <rbgParser/src/game_move.hpp>
#include "game_description/moves/moves.h"

namespace rbg {
  struct VertexNode {
    vertex_id_t vertex;
    node_t node;

    bool operator==(const VertexNode &b) const {
      return node == b.node && vertex == b.vertex;
    }
  };
}

namespace std {
  template<>
  struct hash<rbg::VertexNode> {
    std::size_t operator()(const rbg::VertexNode &node_vertex_pair) const {
      return hash<rbg::node_t>()(node_vertex_pair.node) ^ hash<rbg::vertex_id_t>()(node_vertex_pair.vertex);
    }
  };
}

namespace rbg {
  struct VisitedChecksNfa {
    uint visited_checks_count;
    Nfa<std::unique_ptr<Move>> nfa;
  };

  struct GraphBoardMultiplication {
    explicit GraphBoardMultiplication(const Nfa<std::unique_ptr<Move>> &nfa) : original_nfa(nfa) {}

    const Nfa<std::unique_ptr<Move>> &original_nfa;
    Graph<const Move *> graph_with_board{};
    std::unordered_map<VertexNode, node_t> node_mapping{};
    std::unordered_map<node_t, VertexNode> reverse_node_mapping{};
  };

  GraphBoardMultiplication CreateNfaBoardMultiplication(const Nfa<std::unique_ptr<Move>> &nfa,
                                                        const Declarations &declarations);

  Nfa<std::unique_ptr<Move>> ThompsonsConstruction(const rbg_parser::game_move &rbg_move,
                                                   const Declarations &declarations);

  Nfa<std::unique_ptr<Move>> CreateNfa(const rbg_parser::game_move &rbg_move, const Declarations &declarations);
}



#endif //RBGGAMEMANAGER_GRAPH_CREATOR_H
