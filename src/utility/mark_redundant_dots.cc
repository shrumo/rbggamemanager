#include "mark_redundant_dots.h"

namespace rbg {

bool IsModifierNode(const Nfa<std::unique_ptr<Move>> &nfa,
                    const NfaBoardProduct &nfa_board, node_t vertex_node) {
  return nfa_board.edges_ids_from(vertex_node).size() == 1 &&
         IsModifier(
             nfa.graph
                 .GetEdge(nfa_board.EdgesFrom(vertex_node).begin()->content())
                 .content()
                 ->type());
}

bool MultipleModifierPathsForward(const Nfa<std::unique_ptr<Move>> &nfa,
                                  const NfaBoardProduct &board_product,
                                  node_t noop_vertex_node) {
  std::unordered_set<node_t> visited;
  std::queue<node_t> to_visit;

  visited.insert(noop_vertex_node);
  to_visit.push(noop_vertex_node);
  while (!to_visit.empty()) {
    node_t current = to_visit.front();
    to_visit.pop();
    for (const auto &edge : board_product.EdgesFrom(current)) {
      if (visited.find(edge.to()) != visited.end()) {
        if (IsModifierNode(nfa, board_product, edge.to())) {
          return true;
        }
        continue;
      }

      to_visit.push(edge.to());
      visited.insert(edge.to());
    }
  }
  return false;
}

bool MultipleModifierPathsBackward(const Nfa<std::unique_ptr<Move>> &nfa,
                                   const NfaBoardProduct &board_product,
                                   node_t noop_vertex_node) {
  std::unordered_set<node_t> visited;
  std::queue<node_t> to_visit;

  visited.insert(noop_vertex_node);
  to_visit.push(noop_vertex_node);
  while (!to_visit.empty()) {
    node_t current = to_visit.front();
    to_visit.pop();
    for (const auto &edge : board_product.EdgesTo(current)) {
      if (visited.find(edge.to()) != visited.end()) {
        if (IsModifierNode(nfa, board_product, edge.from())) {
          return true;
        }
        continue;
      }

      to_visit.push(edge.from());
      visited.insert(edge.from());
    }
  }
  return false;
}

bool IsNoopRedundant(const NfaBoardProduct &board_product, node_t nfa_noop_node,
                     size_t vertices_count) {
  bool redundant = true;
  for (vertex_id_t vertex = 1; vertex < vertices_count; vertex++) {
    if (board_product.vertex_node_exists(nfa_noop_node, vertex)) {
      node_t node = board_product.node(nfa_noop_node, vertex);

      if (MultipleModifierPathsForward || MultipleModifierPathsBackward) {
        redundant = false;
      }
    }
  }
  return redundant;
}

std::unordered_set<const rbg_parser::game_move *> RedundantNoopsInGame(
    const std::string &game_text) {
  auto game = ParseGame(game_text);
  Declarations declarations = Declarations(*game);
  rbg::Nfa<std::unique_ptr<rbg::Move>> nfa =
      CreateNfa(*game->get_moves(), declarations);
  NfaBoardProduct board_product(nfa, declarations.initial_board(),
                                {/* vertex_id= */ 1});

  std::unordered_set<const rbg_parser::game_move *> result;
  for (node_t node : GetNoops(nfa)) {
    if (IsNoopRedundant(board_product, node,
                        declarations.initial_board().vertices_count())) {
      result.insert(
          nfa.graph.EdgesFrom(node).begin()->content()->original_move());
    }
  }
  return result;
}
}  // namespace rbg
