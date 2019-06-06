//
// Created by shrum on 28.05.19.
//

#include "graph_creator.h"

#include <graph/graph.h>
#include "parser/parser_wrapper.h"
#include "parser/parser_actions.h"

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
  explicit GraphCreator(Graph<std::unique_ptr<Move>> &graph) : graph_(graph) {}

  GraphCreatorResult SumCase(const rbg_parser::sum &sum) override {
    node_t initial = graph_.AddNode();
    node_t final = graph_.AddNode();
    for (const std::unique_ptr<Move> &child : sum.get_content()) {
      GraphCreatorResult child_result = GraphCreator(graph_)(*child);
      graph_.IdentifyNodes(initial, child_result.initial);
      graph_.IdentifyNodes(final, child_result.final);
    }
    return {initial, final};
  }

  GraphCreatorResult PrioritizedSumCase(const rbg_parser::prioritized_sum &) override {
    throw std::logic_error{"Function not yet implemented."};
  }

  GraphCreatorResult ConcatenationCase(const rbg_parser::concatenation &concatenation) override {
    node_t initial = graph_.AddNode();
    node_t last_final = initial;
    for (const std::unique_ptr<Move> &child : concatenation.get_content()) {
      GraphCreatorResult child_result = GraphCreator(graph_)(*child);
      graph_.IdentifyNodes(last_final, child_result.initial);
      last_final = child_result.final;
    }
    return {initial, last_final};
  }

  GraphCreatorResult StarCase(const rbg_parser::star_move &move) override {
    node_t initial = graph_.AddNode();
    node_t final = graph_.AddNode();
    GraphCreatorResult child_result = GraphCreator(graph_)(*move.get_content());
    graph_.AddEdge(initial, e(), child_result.initial);
    graph_.AddEdge(child_result.final, e(), final);
    graph_.AddEdge(child_result.final, e(), child_result.initial);
    graph_.AddEdge(initial, e(), final);
    return {initial, final};
  }

  GraphCreatorResult GameMoveCase(const Move &m) override {
    node_t initial = graph_.AddNode();
    node_t final = graph_.AddNode();
    edge_id_t edge_id = graph_.AddEdge(initial, m.copy(), final);
    graph_.edge_content(edge_id)->give_indices_in_expression(m.index_in_expression());
    return {initial, final};
  }

private:
  Graph<std::unique_ptr<Move> > &graph_;
};

Nfa<std::unique_ptr<Move>> rbg::CreateGraph(const Move &move) {
  Graph<std::unique_ptr<Move>> graph;
  GraphCreatorResult result = GraphCreator(graph)(move);
  return {std::move(graph), result.initial, result.final};
}