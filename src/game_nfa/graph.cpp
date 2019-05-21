//
// Created by shrum on 21.05.19.
//

#include "graph.h"
#include <algorithm>

void graph::Graph::DeleteNode(graph::node_t node) {
  if (nodes_.find(node) == nodes_.end()) {
    throw std::logic_error("Node " + std::to_string(node) + " doesn't exist.");
  }
  std::unordered_set<edge_id_t> edges_to_delete;
  edges_to_delete.insert(out_edges_[node].begin(), out_edges_[node].end());
  edges_to_delete.insert(in_edges_[node].begin(), in_edges_[node].end());
  for (edge_id_t edge_id : edges_to_delete) {
    DeleteEdge(edge_id);
  }
  nodes_.erase(node);
}

void graph::Graph::DeleteEdge(graph::edge_id_t edge_id) {
  if (edges_.find(edge_id) == edges_.end()) {
    throw std::logic_error("Node " + std::to_string(edge_id) + " doesn't exist.");
  }
  {
    const Edge &edge = edges_[edge_id];
    out_edges_[edge.from].erase(edge_id);
    in_edges_[edge.to].erase(edge_id);
  }
  edges_.erase(edge_id);
}

std::vector<graph::Transition> graph::Graph::InTransitions(graph::node_t node) const {
  if (nodes_.find(node) == nodes_.end()) {
    throw std::logic_error("Node " + std::to_string(node) + " doesn't exist.");
  }
  std::vector<Transition> result;
  for (edge_id_t edge_id : in_edges_.at(node)) {
    const Edge &edge = edges_.at(edge_id);
    result.push_back(Transition{edge_id, edge.from, edge.action});
  }
  return result;
}

std::vector<graph::Transition> graph::Graph::Transitions(graph::node_t node) const {
  if (nodes_.find(node) == nodes_.end()) {
    throw std::logic_error("Node " + std::to_string(node) + " doesn't exist.");
  }
  std::vector<Transition> result;
  for (edge_id_t edge_id : out_edges_.at(node)) {
    const Edge &edge = edges_.at(edge_id);
    result.push_back(Transition{edge_id, edge.to, edge.action});
  }
  return result;
}

graph::edge_id_t graph::Graph::AddEdge(graph::node_t from, const Action *action, graph::node_t to) {
  if (nodes_.find(from) == nodes_.end()) {
    throw std::logic_error("Node " + std::to_string(from) + " doesn't exist.");
  }
  if (nodes_.find(to) == nodes_.end()) {
    throw std::logic_error("Node " + std::to_string(to) + " doesn't exist.");
  }
  edge_id_t transition_id = next_transition_id_;
  next_transition_id_++;
  edges_[transition_id].from = from;
  edges_[transition_id].action = action;
  edges_[transition_id].to = to;
  out_edges_[from].insert(transition_id);
  in_edges_[to].insert(transition_id);
  return transition_id;
}

graph::node_t graph::Graph::AddNode() {
  node_t node = next_node_;
  next_node_++;
  nodes_.insert(node);
  edges_[node];
  return node;
}

std::ostream &operator<<(std::ostream &o, const graph::Graph &g) {
  std::vector<graph::node_t> nodes(g.nodes().begin(), g.nodes().end());
  std::sort(nodes.begin(), nodes.end());
  for (graph::node_t node : nodes) {
    o << node << " -> ";
    auto transitions = g.Transitions(node);
    std::sort(transitions.begin(), transitions.end(),
              [](const graph::Transition &a, const graph::Transition &b) { return a.node < b.node; });
    for (const auto &transition : transitions) {
      o << transition.node << " (edge id: " << transition.id << "), ";
    }
    o << "\n";
  }
  return o;
}
