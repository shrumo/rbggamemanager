//
// Created by shrum on 21.05.19.
//
#include <utility/graph.h>

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace rbg;

std::vector<node_t> SortedOutNeighbours(const Graph<int> &g, node_t node) {
  std::vector<node_t> result;
  for (const auto &edge : g.EdgesFrom(node)) {
    result.push_back(edge.to());
  }
  std::sort(result.begin(), result.end());
  return result;
}

std::vector<node_t> SortedInNeighbours(const Graph<int> &g, node_t node) {
  std::vector<node_t> result;
  for (const auto &edge : g.EdgesTo(node)) {
    result.push_back(edge.from());
  }
  std::sort(result.begin(), result.end());
  return result;
}

int main() {
  Graph<int> g;
  node_t v1 = g.NewNode();
  node_t v2 = g.NewNode();
  node_t v3 = g.NewNode();

  g.AddEdge(v1, 0, v2);
  g.AddEdge(v2, 1, v3);
  g.AddEdge(v3, 2, v1);
  g.AddEdge(v1, 3, v3);

  assert(g.nodes_map().size() == 3);
  assert(g.edges_map().size() == 4);
  assert(SortedOutNeighbours(g, v1) == std::vector<node_t>({v2, v3}));
  assert(SortedOutNeighbours(g, v3) == std::vector<node_t>({v1}));

  node_t v4 = g.NewNode();
  g.AddEdge(v4, 4, v1);

  assert(g.nodes_map().size() == 4);
  assert(g.edges_map().size() == 5);
  std::cout << GraphDescription(g, [](const int &a) {
    return std::to_string(a);
  }) << std::endl;
  assert(SortedInNeighbours(g, v1) == std::vector<node_t>({v3, v4}));
  assert(SortedInNeighbours(g, v2) == std::vector<node_t>({v1}));

  assert(g.edges_ids_from(v3).size() == 1);
  edge_id_t v3v1edge = g.edges_ids_from(v3).front();
  g.EraseEdge(v3v1edge);
  assert(SortedInNeighbours(g, v1) == std::vector<node_t>({v4}));
  assert(SortedOutNeighbours(g, v3).empty());

  g.EraseNode(v1);
  assert(g.nodes_map().size() == 3);
  assert(SortedOutNeighbours(g, v4).empty());
  assert(SortedOutNeighbours(g, v3).empty());
  assert(SortedInNeighbours(g, v2).empty());
  assert(SortedOutNeighbours(g, v2) == std::vector<node_t>({v3}));

  g.AddEdge(v3, 5, v4);
  g.AddEdge(v3, 6, v3);
  g.AddEdge(v4, 7, v3);
  g.AddEdge(v4, 8, v2);
  g.MergeWithNode(v2, v3);
  assert(SortedOutNeighbours(g, v2) == std::vector<node_t>({v2, v2, v4}));

  std::cout << GraphDescription(g,
                                [](const int &a) { return std::to_string(a); });
}