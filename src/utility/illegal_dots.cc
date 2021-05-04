#include "utility/illegal_dots.h"
#include "utility/printer.h"

#include <cassert>
#include <chrono>
#include <fstream>
#include <queue>
#include <random>

namespace rbg {

using namespace std;

struct PairHash
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

// Algorithm used here is starting a DFS from each modifier in the board product
// graph. It remembers the dots encountered and saves them for each vertex node
// of the board product. If it encounters a node that can be reached with two
// different sets of dots, then those dots must be illegal.

// This is the dfa state used for the FindIllegalNoopEdgesRec function.
struct DfaState {
  unordered_set<node_t> finished;
  unordered_map<node_t, int> visited_in_times;
  unordered_map<
      node_t, shared_ptr<unordered_set<pair<int, /*board_product*/ edge_id_t>, PairHash>>>
      visited_dot_applications;
  shared_ptr<unordered_set<pair<int, /*board_product*/ edge_id_t>, PairHash>>
      current_dots_applications =
          make_shared<unordered_set<pair<int, /*board_product*/ edge_id_t>, PairHash>>();
  int current_time = 0;

  unordered_set<edge_id_t> result;
};

void FindIllegalNoopEdgesRec(const NfaBoardProduct &board_product,
                             /*board_product*/ node_t current_node,
                             DfaState &dfa_state) {
  if (dfa_state.visited_in_times.find(current_node) !=
      dfa_state.visited_in_times.end()) {
    return;
  }

  dfa_state.visited_in_times[current_node] = dfa_state.current_time;
  dfa_state.current_time++;
  dfa_state.visited_dot_applications[current_node] =
      dfa_state.current_dots_applications;

  for (const Edge</*nfa*/ edge_id_t> &edge :
       board_product.EdgesFrom(current_node)) {

    shared_ptr<unordered_set<pair<int, /*board_product*/ edge_id_t>, PairHash>>
        previous_dots_applications = dfa_state.current_dots_applications;

    if (board_product.original_nfa()
            ->graph.GetEdge(edge.content())
            .content()
            ->type() == MoveType::kNoop) {
      dfa_state.current_dots_applications =
          make_shared<unordered_set<pair<int, /*board_product*/ edge_id_t>, PairHash>>(
              dfa_state.current_dots_applications->begin(),
              dfa_state.current_dots_applications->end());
      dfa_state.current_dots_applications->insert(
          {dfa_state.visited_in_times[current_node], edge.id()});
    }

    if (dfa_state.visited_in_times.find(edge.to()) !=
        dfa_state.visited_in_times.end()) {
      // Loop case
      if (dfa_state.finished.find(edge.to()) == dfa_state.finished.end()) {
        for (const pair<int, /*board_product*/ edge_id_t> &dot_application :
             *dfa_state.current_dots_applications) {
          int in_time = dfa_state.visited_in_times.at(edge.to());
          if (dot_application.first > in_time) {
            // The dot is in a loop, mark it as illegal
            dfa_state.result.insert(
                board_product.GetEdge(dot_application.second).content());
          }
        }
        goto cleanup;
      }

      shared_ptr<unordered_set<pair<int, /*board_product*/ edge_id_t>, PairHash>>
          &other_dots_applications =
              dfa_state.visited_dot_applications.at(edge.to());

      if (other_dots_applications != dfa_state.current_dots_applications) {
        // We need to calculate unique elements in both sets (symmetric
        // difference)
        for (const pair<int, /*board_product*/ edge_id_t> &dot_application :
             *other_dots_applications) {
          if (dfa_state.current_dots_applications->find(dot_application) ==
              dfa_state.current_dots_applications->end()) {
            dfa_state.result.insert(
                board_product.GetEdge(dot_application.second).content());
          }
        }
        for (const pair<int, /*board_product*/ edge_id_t> dot_application :
             *dfa_state.current_dots_applications) {
          if (other_dots_applications->find(dot_application) ==
              other_dots_applications->end()) {
            dfa_state.result.insert(
                board_product.GetEdge(dot_application.second).content());
          }
        }
        goto cleanup;
      }
    }

    if (!IsModifier(board_product.original_nfa()
                        ->graph.GetEdge(edge.content())
                        .content()
                        ->type())) {
      FindIllegalNoopEdgesRec(board_product, edge.to(), dfa_state);
    }

  cleanup:
    dfa_state.current_dots_applications = previous_dots_applications;
  }
  dfa_state.finished.insert(current_node);
}

unordered_set<const rbg_parser::game_move *>
FindIllegalNoops(const Nfa<unique_ptr<Move>> &nfa,
                 const Declarations &declarations) {

  NfaBoardProduct board_product(nfa, declarations.initial_board(),
                                /*initials=*/{/*vertex_id=*/1});

  unordered_set<const rbg_parser::game_move *> result;
  unordered_set<node_t> explored_nodes;

  // Explore starting from the initial node explictly
  node_t initial_board_product_node =
      board_product.node(/*vertex=*/1, nfa.initial);

  DfaState dfa_state;
  FindIllegalNoopEdgesRec(board_product, initial_board_product_node, dfa_state);

  for (edge_id_t edge : dfa_state.result) {
    result.insert(nfa.graph.GetEdge(edge).content()->original_move());
  }

  explored_nodes.insert(initial_board_product_node);

  // Explore starting from each modifier (as the dfs stops at modifiers)
  for (/*board_product*/ node_t node : board_product.nodes()) {
    for (const Edge<edge_id_t> &edge : board_product.EdgesTo(node)) {
      if (IsModifier(board_product.original_nfa()
                         ->graph.GetEdge(edge.content())
                         .content()
                         ->type())) {
        if (explored_nodes.find(node) != explored_nodes.end()) {
          continue;
        }
        DfaState dfa_state;
        FindIllegalNoopEdgesRec(board_product, node, dfa_state);

        for (edge_id_t edge : dfa_state.result) {
          result.insert(nfa.graph.GetEdge(edge).content()->original_move());
        }
        explored_nodes.insert(node);
      }
    }
  }

  return result;
}

} // namespace rbg