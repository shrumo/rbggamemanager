//
// Created by shrum on 28.05.19.
//

#include "graph_creator.h"

#include <graph/graph.h>
#include "parser/parser_wrapper.h"
#include "parser/parser_actions.h"
#include "construction/moves/moves_creator.h"

#include <memory>

using namespace rbg;

struct GraphCreatorResult {
  node_t initial;
  node_t final;
};

std::unique_ptr<Move> e() {
  return nullptr;
}

// GraphCreator is responsible for applying the Thompson construction to the parsed abstract syntax tree.
// It creates the actions and gives them appropriate numbers that come from the order they appeared in the game
// description.
class GraphCreator : public AstFunction<GraphCreatorResult> {
public:
  explicit GraphCreator(Graph<std::unique_ptr<Move>> &graph, const Declarations &declarations)
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
  Graph<std::unique_ptr<Move> > &graph_;
  const Declarations &declarations_;
};

Nfa<std::unique_ptr<Move>> rbg::CreateGraph(const rbg_parser::game_move &move, const Declarations &declarations) {
  Graph<std::unique_ptr<Move>> graph;
  GraphCreatorResult result = GraphCreator(graph, declarations)(move);
  return {std::move(graph), result.initial, result.final};
}