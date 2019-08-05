//
// Created by shrum on 28.05.19.
//

#include "graph_creator.h"

#include <utility/graph.h>
#include "parser/parser_wrapper.h"
#include "parser/parser_actions.h"
#include "game_description/construction/moves_creator.h"
#include <memory>
#include <utility/moves_visitor.h>
#include <queue>

using namespace std;
using namespace rbg;

struct GraphCreatorResult {
  node_t initial;
  node_t final;
};

unique_ptr<Move> e() {
  return make_unique<Empty>();
}

// GraphCreator is responsible for applying the Thompson construction to the parsed abstract syntax tree.
// It creates the actions and gives them appropriate numbers that come from the order they appeared in the game
// description.
class GraphCreator : public AstFunction<GraphCreatorResult> {
public:
  explicit GraphCreator(Graph<unique_ptr<Move>> &graph, const Declarations &declarations)
      : graph_(graph), declarations_(declarations) {}

  GraphCreatorResult SumCase(const rbg_parser::sum &sum) override {
    node_t initial = graph_.NewNode();
    node_t final = graph_.NewNode();
    for (const auto &child : sum.get_content()) {
      GraphCreatorResult child_result = GraphCreator(graph_, declarations_)(*child);
      graph_.MergeWithNode(initial, child_result.initial);
      graph_.MergeWithNode(final, child_result.final);
    }
    return {initial, final};
  }

  GraphCreatorResult PrioritizedSumCase(const rbg_parser::prioritized_sum &) override {
    assert(false);
  }

  GraphCreatorResult ConcatenationCase(const rbg_parser::concatenation &concatenation) override {
    node_t initial = graph_.NewNode();
    node_t last_final = initial;
    for (const auto &child : concatenation.get_content()) {
      GraphCreatorResult child_result = GraphCreator(graph_, declarations_)(*child);
      graph_.MergeWithNode(last_final, child_result.initial);
      last_final = child_result.final;
    }
    return {initial, last_final};
  }

  GraphCreatorResult StarCase(const rbg_parser::star_move &move) override {
    node_t initial = graph_.NewNode();
    node_t final = graph_.NewNode();
    GraphCreatorResult child_result = GraphCreator(graph_, declarations_)(*move.get_content());
    graph_.AddEdge(initial, e(), child_result.initial);
    graph_.AddEdge(child_result.final, e(), final);
    graph_.AddEdge(child_result.final, e(), child_result.initial);
    graph_.AddEdge(initial, e(), final);
    return {initial, final};
  }

  GraphCreatorResult GameMoveCase(const rbg_parser::game_move &m) override {
    // Move representation graph edge
    node_t initial = graph_.NewNode();
    node_t final = graph_.NewNode();
    graph_.AddEdge(initial, CreateMove(m, declarations_), final);
    return {initial, final};
  }

private:
  Graph<unique_ptr<Move> > &graph_;
  const Declarations &declarations_;
};

Nfa<unique_ptr<Move>> rbg::ThompsonsConstruction(const rbg_parser::game_move &rbg_move, const Declarations &declarations) {
  Graph<unique_ptr<Move>> graph;
  GraphCreatorResult result = GraphCreator(graph, declarations)(rbg_move);
  return Nfa<unique_ptr<Move>>{std::move(graph), result.initial, result.final};
}

// Change this to take board instead.
GraphBoardMultiplication rbg::CreateNfaBoardMultiplication(const Nfa<std::unique_ptr<Move>> &nfa,
                                                           const Declarations &declarations) {
  GraphBoardMultiplication result(nfa);
  queue<VertexNode> to_visit;
  to_visit.push({1,nfa.initial});
  result.node_mapping[{1,nfa.initial}] = result.graph_with_board.NewNode();
  while (!to_visit.empty()) {
    auto top = to_visit.front();
    to_visit.pop();
    for (edge_id_t transition_id : nfa.graph.edges_ids_from(top.node)) {
      const auto& transition = nfa.graph.GetEdge(transition_id);
      vector<vertex_id_t> reachable_vertices;
      if (transition.content()->type() == MoveType::kShiftType) {
        edge_id_t edge_id = dynamic_cast<Shift *>(transition.content().get())->edge_id();
        vertex_id_t next = declarations.initial_board().NextVertex(top.vertex, edge_id);
        if (next != declarations.initial_board().vertices_count()) {
          reachable_vertices.push_back(next);
        }
      } else if (transition.content()->type() == MoveType::kShiftTableType) {
        const auto &table = dynamic_cast<ShiftTable *>(transition.content().get())->table();
        reachable_vertices.insert(reachable_vertices.end(), table[top.vertex].begin(), table[top.vertex].end());
      } else {
        reachable_vertices.push_back(top.vertex);
      }
      for (vertex_id_t vertex : reachable_vertices) {
        VertexNode next_node_vertex_pair{vertex, transition.to()};
        if (result.node_mapping.find(next_node_vertex_pair) == result.node_mapping.end()) {
          result.node_mapping[next_node_vertex_pair] = result.graph_with_board.NewNode();
          to_visit.push(next_node_vertex_pair);
          result.reverse_node_mapping[result.node_mapping[next_node_vertex_pair]] = next_node_vertex_pair;
        }
        node_t current_node = result.node_mapping[top];
        node_t next_node = result.node_mapping[next_node_vertex_pair];
        result.graph_with_board.AddEdge(current_node, transition.content().get(), next_node);
      }
    }
  }
  return result;
}


uint AddVisitedChecks(Nfa<unique_ptr<Move>> &nfa, const Declarations &declarations, uint visited_checks_index = 0) {
  auto graph_times_board = CreateNfaBoardMultiplication(nfa, declarations);
  queue<node_t> to_start_searches_from;
  unordered_map<node_t, node_t> last_visited_by_search_from;
  for (const auto& pair : graph_times_board.graph_with_board.nodes_map()) {
    node_t node = pair.first;
    last_visited_by_search_from[node] = declarations.initial_board().vertices_count();
  }
  unordered_map<node_t, bool> should_add_check(nfa.graph.nodes_map().size());
  to_start_searches_from.push(graph_times_board.node_mapping[{nfa.initial, 0}]);
  while (!to_start_searches_from.empty()) {
    node_t search_from = to_start_searches_from.front();
    to_start_searches_from.pop();
    queue<node_t> to_visit;
    to_visit.push(search_from);
    last_visited_by_search_from[search_from] = search_from;
    while (!to_visit.empty()) {
      node_t current = to_visit.front();
      to_visit.pop();
      for (edge_id_t transition_id : graph_times_board.graph_with_board.edges_ids_from(current)) {
        const auto& transition = graph_times_board.graph_with_board.GetEdge(transition_id);
        if (last_visited_by_search_from[transition.to()] == declarations.initial_board().vertices_count()) {
          if (is_modifier_type(transition.content()->type())) {
            to_start_searches_from.push(transition.to());
            last_visited_by_search_from[transition.to()] = transition.to();
          } else {
            to_visit.push(transition.to());
          }
        }
        if (last_visited_by_search_from[transition.to()] == search_from) {
          should_add_check[graph_times_board.reverse_node_mapping[transition.to()].node] = true;
        }
        last_visited_by_search_from[transition.to()] = search_from;
      }
    }
  }
  queue<node_t> to_visit;
  unordered_set<node_t> visited;
  to_visit.push(nfa.initial);
  visited.insert(nfa.initial);

  while (!to_visit.empty()) {
    node_t u = to_visit.front();
    to_visit.pop();
    for (edge_id_t transition_id : nfa.graph.edges_ids_from(u)) {
      auto& transition = nfa.graph.GetEdge(transition_id);
      node_t v = transition.to();
      if (transition.content()->type() == MoveType::kConditionCheck) {
        visited_checks_index = AddVisitedChecks(dynamic_cast<Condition *>(transition.modifiable_content().get())->nfa(),
                                                declarations, visited_checks_index);
      }
      if (visited.find(transition.to()) == visited.end()) {
        if (should_add_check[transition.to()]) {
          node_t visited_check = nfa.graph.NewNode();
          unordered_set<edge_id_t> transitions_to = nfa.graph.edges_ids_to(transition.to());
          for (edge_id_t in_transition_id : transitions_to) {
            nfa.graph.ChangeEdgeTarget(in_transition_id, visited_check);
          }
          nfa.graph.AddEdge(visited_check, make_unique<VisitedQuery>(visited_checks_index), v);
          visited_checks_index++;
          to_visit.push(v);
          visited.insert(v);
          visited.insert(visited_check);
        } else {
          to_visit.push(v);
          visited.insert(v);
        }
      }
    }
  }
  return visited_checks_index;
}

void HandleMultipleOutNodes(Graph<unique_ptr<Move>> &graph) {
  for (node_t node: std::vector<node_t>(graph.nodes().begin(), graph.nodes().end())) {
    auto out_transitions = graph.edges_ids_from(node);
    if (out_transitions.size() > 1) {
      for (edge_id_t transition_id : out_transitions) {
        auto& transition = graph.GetEdge(transition_id);
        if (transition.content()->type() != MoveType::kEmpty) {
          node_t new_node = graph.NewNode();
          graph.AddEdge(node, e(), new_node);
          graph.ChangeEdgeSource(transition_id, new_node);
        }
      }
      std::unordered_set<node_t> handled;
      std::vector<edge_id_t> to_erase;
      for (edge_id_t transition_id :  graph.edges_ids_from(node)) {
        const auto& transition = graph.GetEdge(transition_id);
        if (handled.find(transition.to()) != handled.end()) {
          to_erase.push_back(transition_id);
        } else {
          handled.insert(transition_id);
        }
      }
      for(edge_id_t transition_id : to_erase) {
        graph.EraseEdge(transition_id);
      }
    }
  }
  for (const auto& pair : graph.nodes_map()) {
    node_t node = pair.first;
    auto out_transitions = graph.edges_ids_from(node);
    for (edge_id_t transition_id : out_transitions) {
      auto& transition = graph.GetEdge(transition_id);
      if (transition.content()->type() == MoveType::kConditionCheck) {
        HandleMultipleOutNodes(dynamic_cast<Condition *>(transition.modifiable_content().get())->nfa().graph);
      }
    }
  }
}

VisitedChecksNfa rbg::CreateVisitedChecksNfa(const rbg_parser::game_move &rbg_move, const Declarations &declarations) {
  auto nfa = ThompsonsConstruction(rbg_move, declarations);
  uint visited_checks_count = AddVisitedChecks(nfa, declarations);
  HandleMultipleOutNodes(nfa.graph);
  return VisitedChecksNfa{visited_checks_count, move(nfa)};
}