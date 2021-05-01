//
// Created by shrum on 28.05.19.
//

#ifndef RBGGAMEMANAGER_GRAPH_CREATOR_H
#define RBGGAMEMANAGER_GRAPH_CREATOR_H

#include <utility/graph.h>

#include <memory>
#include <rbgParser/src/game_move.hpp>
#include <utility>

#include "game_description/moves/moves.h"

namespace rbg {
// On edge of the graph are ids of original edges and a move that corresponds to
// that edge.
class NfaBoardProduct : public Graph<edge_id_t> {
public:
  explicit NfaBoardProduct(const Nfa<std::unique_ptr<Move>> &nfa,
                           const Board &board,
                           const std::vector<vertex_id_t> &initials);

  // Returns the node of the NfaBoardProduct that corresponds to this vertex
  // node pair.
  node_t node(vertex_id_t vertex, node_t node) const {
    return node_mapping_.at({vertex, node});
  }

  // Returns the vertex node pair that this NfaBoardProduct node corresponds to.
  std::pair<vertex_id_t, node_t> vertex_node(node_t node) const {
    const auto &result = reverse_node_mapping_.at(node);
    return {result.vertex, result.node};
  }

  bool vertex_node_exists(vertex_id_t vertex, node_t node) const {
    return node_mapping_.find({vertex, node}) != node_mapping_.end();
  }

  const NfaBoardProduct &sub_nfa_board(node_t condition_initial_node) const {
    return *sub_graphs_.at(condition_initial_node);
  }

private:
  struct VertexNode {
    vertex_id_t vertex;
    node_t node;

    bool operator==(const VertexNode &b) const {
      return vertex == b.vertex && node == b.node;
    }
  };

  struct VertexNodeHash {
    std::size_t operator()(const VertexNode &v) const {
      return v.vertex * 5693 + v.node;
    }
  };

  std::unordered_map<node_t, std::unique_ptr<NfaBoardProduct>> sub_graphs_;
  std::unordered_map<VertexNode, node_t, VertexNodeHash> node_mapping_;
  // This is the translation between nodes of the nfa board graph to vertex node
  // pairs
  std::unordered_map<node_t, VertexNode> reverse_node_mapping_;
};

Nfa<std::unique_ptr<Move>> CreateNfa(const rbg_parser::game_move &rbg_move,
                                     const Declarations &declarations,
                                     bool optimize = true);
std::vector<node_t> GetNoops(const Nfa<std::unique_ptr<Move>> &nfa);
} // namespace rbg

#endif // RBGGAMEMANAGER_GRAPH_CREATOR_H
