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
  while(board_product.edges_ids_from(noop_vertex_node).size() == 1 && !IsModifierNode(nfa, board_product, noop_vertex_node)) {
    noop_vertex_node = board_product.EdgesFrom(noop_vertex_node).begin()->to();
  }

  return board_product.edges_ids_from(noop_vertex_node).size() > 1;
}

bool MultipleModifierPathsBackward(const Nfa<std::unique_ptr<Move>> &nfa,
                                   const NfaBoardProduct &board_product,
                                   node_t noop_vertex_node) {
  std::unordered_set<node_t> visited;
  std::queue<node_t> to_visit;

   while(board_product.edges_ids_to(noop_vertex_node).size() == 1 && !IsModifierNode(nfa, board_product, noop_vertex_node)) {
    noop_vertex_node = board_product.EdgesTo(noop_vertex_node).begin()->from();
  }

  return board_product.edges_ids_to(noop_vertex_node).size() > 1;
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
