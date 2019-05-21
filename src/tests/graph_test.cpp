//
// Created by shrum on 21.05.19.
//

#include <game_nfa/graph.h>
#include <algorithm>
#include "test.h"

using namespace graph;

std::vector<node_t> SortedTransitions(const Graph &g, node_t node) {
  auto transitions = g.Transitions(node);
  std::vector<node_t> result;
  result.reserve(transitions.size());
  for (const auto &t : transitions) {
    result.push_back(t.node);
  }
  std::sort(result.begin(), result.end());
  return result;
}

std::vector<node_t> SortedInTransitions(const Graph &g, node_t node) {
  auto transitions = g.InTransitions(node);
  std::vector<node_t> result;
  result.reserve(transitions.size());
  for (const auto &t : transitions) {
    result.push_back(t.node);
  }
  std::sort(result.begin(), result.end());
  return result;
}

int main() {
  Graph g;
  node_t v1 = g.AddNode();
  node_t v2 = g.AddNode();
  node_t v3 = g.AddNode();

  g.AddEdge(v1, nullptr, v2);
  g.AddEdge(v2, nullptr, v3);
  g.AddEdge(v3, nullptr, v1);
  g.AddEdge(v1, nullptr, v3);

  std::cout << g << std::endl;

  ASSERT(g.nodes().size() == 3)
  ASSERT(SortedTransitions(g, v1) == std::vector<node_t>({v2, v3}))
  ASSERT(SortedTransitions(g, v3) == std::vector<node_t>({v1}))

  node_t v4 = g.AddNode();
  g.AddEdge(v4, nullptr, v1);

  ASSERT(g.nodes().size() == 4)
  ASSERT(SortedInTransitions(g, v1) == std::vector<node_t>({v3, v4}))
  ASSERT(SortedInTransitions(g, v2) == std::vector<node_t>({v1}))

  ASSERT(g.Transitions(v3).size() == 1)
  edge_id_t v3v1edge = g.Transitions(v3).front().id;
  g.DeleteEdge(v3v1edge);
  ASSERT(SortedInTransitions(g, v1) == std::vector<node_t>({v4}))
  ASSERT(SortedTransitions(g, v3).empty())

  g.DeleteNode(v1);
  ASSERT(g.nodes().size() == 3)
  ASSERT(SortedTransitions(g, v4).empty())
  ASSERT(SortedTransitions(g, v3).empty())
  ASSERT(SortedInTransitions(g, v2).empty())
  ASSERT(SortedTransitions(g, v2) == std::vector<node_t>({v3}))
}