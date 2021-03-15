#include "mark_redundant_dots.h"

namespace rbg
{

  // Are the eges leaving from the given vertex_node only modifiers or noops.
  bool IsModifierOrNoopLeavingNode(const Nfa<std::unique_ptr<Move>> &nfa,
                                   const NfaBoardProduct &nfa_board,
                                   node_t vertex_node)
  {
    bool only_dots_or_modifiers_leaving = true;
    for (const Edge<edge_id_t> &edge : nfa_board.EdgesFrom(vertex_node))
    {
      if (!IsModifier(nfa.graph.GetEdge(edge.content()).content()->type()) &&
          nfa.graph.GetEdge(edge.content()).content()->type() != MoveType::kNoop)
      {
        only_dots_or_modifiers_leaving = false;
        break;
      }
    }
    return only_dots_or_modifiers_leaving;
  }

  // Are the eges leaving from the given vertex_node only modifiers or noops.
  bool IsModifierOrNoopEnteringNode(const Nfa<std::unique_ptr<Move>> &nfa,
                                    const NfaBoardProduct &nfa_board,
                                    node_t vertex_node)
  {
    bool only_dots_or_modifiers_entering = true;
    for (const Edge<edge_id_t> &edge : nfa_board.EdgesTo(vertex_node))
    {
      if (!IsModifier(nfa.graph.GetEdge(edge.content()).content()->type()) &&
          nfa.graph.GetEdge(edge.content()).content()->type() != MoveType::kNoop)
      {
        only_dots_or_modifiers_entering = false;
        break;
      }
    }
    return only_dots_or_modifiers_entering;
  }

  bool MultipleModifierPathsForward(const Nfa<std::unique_ptr<Move>> &nfa,
                                    const NfaBoardProduct &board_product,
                                    node_t noop_vertex_node)
  {

    while (board_product.edges_ids_from(noop_vertex_node).size() == 1 &&
           !IsModifierOrNoopLeavingNode(nfa, board_product, noop_vertex_node))
    {
      noop_vertex_node = board_product.EdgesFrom(noop_vertex_node).begin()->to();
    }

    return board_product.edges_ids_from(noop_vertex_node).size() > 1;
  }

  bool MultipleModifierPathsBackward(const Nfa<std::unique_ptr<Move>> &nfa,
                                     const NfaBoardProduct &board_product,
                                     node_t noop_vertex_node)
  {
    while (board_product.edges_ids_to(noop_vertex_node).size() == 1 &&
           !IsModifierOrNoopEnteringNode(nfa, board_product, noop_vertex_node))
    {
      noop_vertex_node = board_product.EdgesTo(noop_vertex_node).begin()->from();
    }

    return board_product.edges_ids_to(noop_vertex_node).size() > 1;
  }

  bool IsNoopRedundant(const Nfa<std::unique_ptr<Move>> &nfa,
                       const NfaBoardProduct &board_product, edge_id_t nfa_noop_edge,
                       size_t vertices_count)
  {
    const Edge<std::unique_ptr<Move>> &noop_edge = nfa.graph.GetEdge(nfa_noop_edge);
    node_t nfa_noop_node = noop_edge.from();
    for (vertex_id_t vertex = 0; vertex < vertices_count; vertex++)
    {
      if (board_product.vertex_node_exists(vertex, nfa_noop_node))
      {
        node_t node = board_product.node(vertex, nfa_noop_node);
        node_t next_node = board_product.EdgesFrom(node).begin()->to();
        if (MultipleModifierPathsForward(nfa, board_product, next_node) &&
            MultipleModifierPathsBackward(nfa, board_product, node))
        {
          return false;
        }
      }
    }
    return true;
  }

  std::unordered_set<const rbg_parser::game_move *> RedundantNoopsInGame(
      const rbg_parser::parsed_game *parsed_game)
  {
    Declarations declarations = Declarations(*parsed_game);
    rbg::Nfa<std::unique_ptr<rbg::Move>> nfa =
        CreateNfa(*parsed_game->get_moves(), declarations, /*optimizer=*/false);
    NfaBoardProduct board_product(nfa, declarations.initial_board(),
                                  {/* vertex_id= */ 1});

    std::unordered_set<const rbg_parser::game_move *> result;
    for (edge_id_t edge_id : GetNoops(nfa))
    {
      if (IsNoopRedundant(nfa, board_product, edge_id,
                          declarations.initial_board().vertices_count()))
      {
        result.insert(
            nfa.graph.GetEdge(edge_id).content()->original_move());
      }
    }
    return result;
  }
} // namespace rbg
