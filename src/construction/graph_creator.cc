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

void AddVisitedChecks(Graph<unique_ptr<Move>> &graph, const Declarations &declarations) {
  for (node_t node : std::vector<node_t>(graph.nodes().begin(), graph.nodes().end())) {
    bool in_shift = false;
    auto in_transitions = graph.InTransitions(node);
    for (const auto &transition : in_transitions) {
      if (NonInjectiveShift(declarations.board_description)(*transition.content)) {
        in_shift = true;
        break;
      }
    }
    if (in_transitions.size() > 1 || in_shift) {
      node_t visited_check_node = graph.AddNode();
      for (const auto &transition : in_transitions) {
        graph.AddEdge(transition.from, std::move(graph.edge_content(transition.id)), visited_check_node);
        graph.DeleteEdge(transition.id);
      }
      graph.AddEdge(visited_check_node, make_unique<VisitedCheck>(), node);
    }
  }
}

Nfa<unique_ptr<Move>> rbg::CreateGraph(const rbg_parser::game_move &rbg_move, const Declarations &declarations) {
  Graph<unique_ptr<Move>> graph;
  GraphCreatorResult result = GraphCreator(graph, declarations)(rbg_move);
  AddVisitedChecks(graph, declarations);
  return {move(graph), result.initial, result.final};
}