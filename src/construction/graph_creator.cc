//
// Created by shrum on 28.05.19.
//

#include "graph_creator.h"

#include <graph/graph.h>
#include "parser/parser_wrapper.h"
#include "parser/parser_actions.h"
#include "moves/moves_creator.h"
#include <memory>
#include <construction/moves/moves_visitor.h>
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
    node_t initial = graph_.AddNode();
    node_t final = graph_.AddNode();
    for (const auto &child : sum.get_content()) {
      GraphCreatorResult child_result = GraphCreator(graph_, declarations_)(*child);
      graph_.IdentifyNodes(initial, child_result.initial);
      graph_.IdentifyNodes(final, child_result.final);
    }
    return {initial, final};
  }

  GraphCreatorResult PrioritizedSumCase(const rbg_parser::prioritized_sum &) override {
    assert(false && "Function not yet implemented.");
  }

  GraphCreatorResult ConcatenationCase(const rbg_parser::concatenation &concatenation) override {
    node_t initial = graph_.AddNode();
    node_t last_final = initial;
    for (const auto &child : concatenation.get_content()) {
      GraphCreatorResult child_result = GraphCreator(graph_, declarations_)(*child);
      graph_.IdentifyNodes(last_final, child_result.initial);
      last_final = child_result.final;
    }
    return {initial, last_final};
  }

  GraphCreatorResult StarCase(const rbg_parser::star_move &move) override {
    node_t initial = graph_.AddNode();
    node_t final = graph_.AddNode();
    GraphCreatorResult child_result = GraphCreator(graph_, declarations_)(*move.get_content());
    graph_.AddEdge(initial, e(), child_result.initial);
    graph_.AddEdge(child_result.final, e(), final);
    graph_.AddEdge(child_result.final, e(), child_result.initial);
    graph_.AddEdge(initial, e(), final);
    return {initial, final};
  }

  GraphCreatorResult GameMoveCase(const rbg_parser::game_move &m) override {
    // Move representation graph edge
    node_t initial = graph_.AddNode();
    node_t final = graph_.AddNode();
    graph_.AddEdge(initial, CreateMove(m, declarations_), final);
    return {initial, final};
  }

private:
  Graph<unique_ptr<Move> > &graph_;
  const Declarations &declarations_;
};

class NonInjectiveShift : public MoveFunction<bool> {
public:
  explicit NonInjectiveShift(const Board &board) : board_(board) {}

  bool ShiftCase(const Shift &move) override {
    vector<bool> seen(board_.vertices_count());
    for (vertex_id_t vertex_id = 0; vertex_id < board_.vertices_count(); vertex_id++) {
      vertex_id_t next = board_.NextVertex(vertex_id, move.edge_id());
      if (next != board_.vertices_count()) {
        if (seen[next]) {
          return true;
        }
        seen[next] = true;
      }
    }
    return false;
  }

  bool ShiftTableCase(const ShiftTable &move) override {
    vector<bool> seen(board_.vertices_count());
    for (vertex_id_t vertex_id = 0; vertex_id < board_.vertices_count(); vertex_id++) {
      for (vertex_id_t next : move.table()[vertex_id]) {
        if (next != board_.vertices_count()) {
          if (seen[next]) {
            return true;
          }
          seen[next] = true;
        }
      }
    }
    return false;
  }

  bool DefaultCase(const Move &) override {
    return false;
  }

private:
  const Board &board_;
};


Nfa<unique_ptr<Move>> rbg::CreateNfa(const rbg_parser::game_move &rbg_move, const Declarations &declarations) {
  Graph<unique_ptr<Move>> graph;
  GraphCreatorResult result = GraphCreator(graph, declarations)(rbg_move);
  return {std::move(graph), result.initial, result.final};
}


GraphTimesBoard rbg::CreateGraphTimesBoard(const Nfa<unique_ptr<Move>> &nfa, const Declarations &declarations) {
  GraphTimesBoard result(nfa);
  queue<NodeVertexPair> to_visit;
  to_visit.push({nfa.initial, 0});
  result.node_mapping[{nfa.initial, 0}] = result.graph_with_board.AddNode();
  while (!to_visit.empty()) {
    NodeVertexPair top = to_visit.front();
    to_visit.pop();
    for (const auto &transition : nfa.graph.Transitions(top.node)) {
      vector<vertex_id_t> reachable_vertices;
      if (transition.content->type() == MoveType::kShiftType) {
        shift_edge_id_t edge_id = dynamic_cast<Shift *>(transition.content.get())->edge_id();
        vertex_id_t next = declarations.board_description.NextVertex(top.vertex, edge_id);
        if (next != declarations.board_description.vertices_count()) {
          reachable_vertices.push_back(next);
        }
      } else if (transition.content->type() == MoveType::kShiftTableType) {
        const auto &table = dynamic_cast<ShiftTable *>(transition.content.get())->table();
        reachable_vertices.insert(reachable_vertices.end(), table[top.vertex].begin(), table[top.vertex].end());
      } else {
        reachable_vertices.push_back(top.vertex);
      }
      for (vertex_id_t vertex : reachable_vertices) {
        NodeVertexPair next_node_vertex_pair = {transition.to, vertex};
        if (result.node_mapping.find(next_node_vertex_pair) == result.node_mapping.end()) {
          result.node_mapping[next_node_vertex_pair] = result.graph_with_board.AddNode();
          to_visit.push(next_node_vertex_pair);
          result.reverse_node_mapping[result.node_mapping[next_node_vertex_pair]] = next_node_vertex_pair;
        }
        node_t current_node = result.node_mapping[top];
        node_t next_node = result.node_mapping[next_node_vertex_pair];
        result.graph_with_board.AddEdge(current_node, transition.content.get(), next_node);
      }
    }
  }
  return result;
}


uint AddVisitedChecks(Nfa<unique_ptr<Move>> &nfa, const Declarations &declarations, uint visited_checks_index = 0) {
  auto graph_times_board = CreateGraphTimesBoard(nfa, declarations);
  queue<node_t> to_start_searches_from;
  unordered_map<node_t, node_t> last_visited_by_search_from;
  for (node_t node : graph_times_board.graph_with_board.nodes()) {
    last_visited_by_search_from[node] = declarations.board_description.vertices_count();
  }
  unordered_map<node_t, bool> should_add_check(nfa.graph.nodes().size());
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
      for (const auto &transition : graph_times_board.graph_with_board.Transitions(current)) {
        if (last_visited_by_search_from[transition.to] == declarations.board_description.vertices_count()) {
          if (is_modifier_type(transition.content->type())) {
            to_start_searches_from.push(transition.to);
            last_visited_by_search_from[transition.to] = transition.to;
          } else {
            to_visit.push(transition.to);
          }
        }
        if (last_visited_by_search_from[transition.to] == search_from) {
          should_add_check[graph_times_board.reverse_node_mapping[transition.to].node] = true;
        }
        last_visited_by_search_from[transition.to] = search_from;
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
    for (const auto &transition : nfa.graph.Transitions(u)) {
      if (transition.content->type() == MoveType::kConditionCheck) {
        visited_checks_index = AddVisitedChecks(dynamic_cast<ConditionCheck *>(transition.content.get())->nfa(),
                                                declarations, visited_checks_index);
      }
      if (visited.find(transition.to) == visited.end()) {
        if (should_add_check[transition.to]) {
          node_t visited_check = nfa.graph.AddNode();
          for (const auto &intransition : nfa.graph.InTransitions(transition.to)) {
            nfa.graph.ChangeTransitionTo(intransition.id, visited_check);
          }
          nfa.graph.AddEdge(visited_check, make_unique<VisitedCheck>(visited_checks_index), transition.to);
          visited_checks_index++;
          to_visit.push(transition.to);
          visited.insert(transition.to);
          visited.insert(visited_check);
        } else {
          to_visit.push(transition.to);
          visited.insert(transition.to);
        }
      }
    }
  }
  return visited_checks_index;
}

void HandleMultipleOutNodes(Graph<unique_ptr<Move>> &graph) {
  for (node_t node : std::vector<node_t>(graph.nodes().begin(), graph.nodes().end())) {
    auto out_transitions = graph.Transitions(node);
    if (out_transitions.size() > 1) {
      for (const auto &transition : out_transitions) {
        if (transition.content->type() != MoveType::kEmpty) {
          node_t new_node = graph.AddNode();
          graph.AddEdge(node, e(), new_node);
          graph.AddEdge(new_node, std::move(graph.edge_content(transition.id)), transition.to);
          graph.DeleteEdge(transition.id);
        }
      }
      std::unordered_set<node_t> handled;
      for (const auto &transition :  graph.Transitions(node)) {
        if (handled.find(transition.to) != handled.end()) {
          graph.DeleteEdge(transition.id);
        } else {
          handled.insert(transition.to);
        }
      }
    }
  }
  for (node_t node : std::vector<node_t>(graph.nodes().begin(), graph.nodes().end())) {
    auto out_transitions = graph.Transitions(node);
    for (const auto &transition : out_transitions) {
      if (graph.edge_content(transition.id)->type() == MoveType::kConditionCheck) {
        HandleMultipleOutNodes(dynamic_cast<ConditionCheck *>(graph.edge_content(transition.id).get())->nfa().graph);
      }
    }
  }
}

NfaWithVisitedChecks rbg::CreateFinalNfa(const rbg_parser::game_move &rbg_move, const Declarations &declarations) {
  auto nfa = CreateNfa(rbg_move, declarations);
  uint visited_checks_count = AddVisitedChecks(nfa, declarations);
  HandleMultipleOutNodes(nfa.graph);
  return {visited_checks_count, move(nfa)};
}