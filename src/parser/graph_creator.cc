//
// Created by shrum on 28.05.19.
//

#include "graph_creator.h"

#include <game_nfa/graph.h>
#include <actions/actions.h>
#include "parser_wrapper.h"
#include "parser_actions.h"
#include "actions_creator.h"

#include <memory>

using namespace rbg;

struct GraphCreatorResult {
  node_t initial;
  node_t final;
  uint next_action_id;
};

std::unique_ptr<Action> e() {
  return std::make_unique<Empty>(0);
}

// GraphCreator is responsible for applying the Thompson construction to the parsed abstract syntax tree.
// It creates the actions and gives them appropriate numbers that come from the order they appeared in the game
// description.
class GraphCreator : public AstVisitorR<GraphCreatorResult> {
public:
  explicit GraphCreator(Graph<std::unique_ptr<Action>> &graph, uint next_action_id) : graph_(graph),
                                                                                      next_action_id_(next_action_id) {}

  GraphCreatorResult visit(const rbg_parser::sum &sum) override {
    node_t initial = graph_.AddNode();
    node_t final = graph_.AddNode();
    for (const std::unique_ptr<rbg_parser::game_move> &child : sum.get_content()) {
      GraphCreatorResult child_result = Fn<GraphCreator, GraphCreatorResult>(*child, graph_, next_action_id_);
      next_action_id_ = child_result.next_action_id;
      graph_.AddEdge(initial, e(), child_result.initial);
      graph_.AddEdge(child_result.final, e(), final);
    }
    return {initial, final, next_action_id_};
  }

  GraphCreatorResult visit(const rbg_parser::prioritized_sum &) override {
    throw std::logic_error{"Function not yet implemented."};
  }

  GraphCreatorResult visit(const rbg_parser::concatenation &concatenation) override {
    node_t initial = graph_.AddNode();
    node_t last_final = initial;
    for (const std::unique_ptr<rbg_parser::game_move> &child : concatenation.get_content()) {
      GraphCreatorResult child_result = Fn<GraphCreator, GraphCreatorResult>(*child, graph_, next_action_id_);
      next_action_id_ = child_result.next_action_id;
      graph_.AddEdge(last_final, e(), child_result.initial);
      last_final = child_result.final;
    }
    return {initial, last_final, next_action_id_};
  }

  GraphCreatorResult visit(const rbg_parser::star_move &move) override {
    node_t initial = graph_.AddNode();
    node_t final = graph_.AddNode();
    GraphCreatorResult child_result = Fn<GraphCreator, GraphCreatorResult>(*move.get_content(), graph_,
                                                                           next_action_id_);
    next_action_id_ = child_result.next_action_id;
    graph_.AddEdge(initial, e(), child_result.initial);
    graph_.AddEdge(child_result.final, e(), final);
    graph_.AddEdge(child_result.final, e(), child_result.initial);
    return {initial, final, next_action_id_};
  }

  GraphCreatorResult gameMoveCase(const rbg_parser::game_move &m) override {
    node_t initial = graph_.AddNode();
    node_t final = graph_.AddNode();
    graph_.AddEdge(initial, CreateAction(m, next_action_id_), final);
    next_action_id_++;
    return {initial, final, next_action_id_};
  }

private:
  Graph<std::unique_ptr<Action> > &graph_;
  uint next_action_id_;
};

Nfa<std::unique_ptr<Action>> rbg::CreateGraph(const rbg_parser::game_move &move) {
  auto graph = std::make_unique<Graph<std::unique_ptr<Action>>>();
  GraphCreatorResult result = Fn<GraphCreator, GraphCreatorResult>(move, *graph, 1);
  return {result.initial, result.final, std::move(graph)};
}