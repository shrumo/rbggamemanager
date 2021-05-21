#include "utility/redundant_dots.h"
#include "utility/printer.h"

#include <cassert>
#include <chrono>
#include <fstream>
#include <queue>
#include <random>

namespace rbg {

bool IsLeftDetermined(const NfaBoardProduct &board_product, node_t node,
                      bool modifiers_as_dots = true) {
  while (board_product.EdgesTo(node).size() == 1) {
    MoveType edge_type =
        board_product.original_nfa()
            ->graph.GetEdge(board_product.EdgesTo(node).begin()->content())
            .content()
            ->type();
    
    if (board_product.EdgesFrom(node).size() > 1) {
      // This means we can walk around the dot
      return false;
    }


    if ((modifiers_as_dots && IsModifier(edge_type)) ||
        IsSwitch(edge_type) ||
        edge_type == MoveType::kNoop) {
      return true;
    }

    node = board_product.EdgesTo(node).begin()->from();

  }

  return board_product.EdgesTo(node).size() <= 1;
}

bool IsRightDetermined(const NfaBoardProduct &board_product, node_t node,
                       bool modifiers_as_dots = true) {
  while (board_product.EdgesFrom(node).size() == 1) {
    MoveType edge_type =
        board_product.original_nfa()
            ->graph.GetEdge(board_product.EdgesFrom(node).begin()->content())
            .content()
            ->type();

    if (board_product.EdgesTo(node).size() > 1) {
      // This means we can walk around the dot
      return false;
    }
    
    if ((modifiers_as_dots && IsModifier(edge_type)) ||
        IsSwitch(edge_type) ||
        edge_type == MoveType::kNoop) {
      return true;
    }

    node = board_product.EdgesFrom(node).begin()->to();
  }
  return board_product.EdgesFrom(node).size() <= 1;
}

std::unordered_map<const rbg_parser::game_move *, NoopDeterminedState>
FindRedundantNoops(const Nfa<std::unique_ptr<Move>> &nfa,
                   const Declarations &declarations, bool modifiers_as_dots) {
  NfaBoardProduct board_product(nfa, declarations.initial_board(),
                                /*initials=*/{/*vertex_id=*/1});

  std::unordered_map<const rbg_parser::game_move *, NoopDeterminedState> result;

  for (edge_id_t edge : GetNoops(nfa)) {
    const Edge<std::unique_ptr<Move>> &nfa_edge = nfa.graph.GetEdge(edge);
    bool left_determined = true;
    bool right_determined = true;
    for (vertex_id_t vertex_id = 1;
         vertex_id < declarations.initial_board().vertices_count();
         vertex_id++) {
      bool locally_left_determined = true;
      if (board_product.vertex_node_exists(vertex_id, nfa_edge.from())) {
        locally_left_determined = IsLeftDetermined(
            board_product, board_product.node(vertex_id, nfa_edge.from()),
            modifiers_as_dots);
      }
      bool locally_right_determined = true;
      if (board_product.vertex_node_exists(vertex_id, nfa_edge.to())) {
        locally_right_determined = IsRightDetermined(
            board_product, board_product.node(vertex_id, nfa_edge.to()),
            modifiers_as_dots);
      }
      left_determined = left_determined && locally_left_determined;
      right_determined = right_determined && locally_right_determined;
    }
    if (left_determined && right_determined) {
      result[nfa_edge.content()->original_move()] =
          NoopDeterminedState::FULLY_DETERMINED;
    } else if (left_determined) {
      result[nfa_edge.content()->original_move()] =
          NoopDeterminedState::LEFT_DETERMINED;
    } else if (right_determined) {

      result[nfa_edge.content()->original_move()] =
          NoopDeterminedState::RIGHT_DETERMINED;
    } else {
      result[nfa_edge.content()->original_move()] =
          NoopDeterminedState::NON_DETERMINED;
    }
  }
  return result;
}
} // namespace rbg
