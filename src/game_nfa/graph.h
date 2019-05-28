//
// Created by shrum on 21.05.19.
//

#ifndef RBGGAMEMANAGER_GRAPH_H
#define RBGGAMEMANAGER_GRAPH_H


#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ostream>
#include <memory>
#include <algorithm>

namespace rbg {
  using node_t = std::size_t;
  using edge_id_t = std::size_t;

  template<typename EdgeContent>
  struct Transition {
    edge_id_t id;
    node_t to;
    EdgeContent content;
  };

  template<typename EdgeContent>
  class Graph {
  public:
    Graph() : next_node_(0), next_transition_id_(0) {}

    node_t AddNode() {
      node_t node = next_node_;
      next_node_++;
      nodes_.insert(node);
      edges_[node];
      return node;
    }

    edge_id_t AddEdge(node_t from, EdgeContent content, node_t to) {
      if (nodes_.find(from) == nodes_.end()) {
        throw std::logic_error("Node " + std::to_string(from) + " doesn't exist.");
      }
      if (nodes_.find(to) == nodes_.end()) {
        throw std::logic_error("Node " + std::to_string(to) + " doesn't exist.");
      }
      edge_id_t transition_id = next_transition_id_;
      next_transition_id_++;
      edges_[transition_id].from = from;
      edges_[transition_id].content = std::move(content);
      edges_[transition_id].to = to;
      out_edges_[from].insert(transition_id);
      in_edges_[to].insert(transition_id);
      return transition_id;
    }

    std::vector<Transition<EdgeContent>> Transitions(node_t node) const {
      if (nodes_.find(node) == nodes_.end()) {
        throw std::logic_error("Node " + std::to_string(node) + " doesn't exist.");
      }
      std::vector<Transition<EdgeContent>> result;
      for (edge_id_t edge_id : out_edges_.at(node)) {
        const Edge &edge = edges_.at(edge_id);
        result.push_back(Transition<EdgeContent>{edge_id, edge.to, edge.content});
      }
      return result;
    }


    std::vector<Transition<EdgeContent>> InTransitions(node_t node) const {
      if (nodes_.find(node) == nodes_.end()) {
        throw std::logic_error("Node " + std::to_string(node) + " doesn't exist.");
      }
      std::vector<Transition<EdgeContent>> result;
      for (edge_id_t edge_id : in_edges_.at(node)) {
        const Edge &edge = edges_.at(edge_id);
        result.push_back(Transition<EdgeContent>{edge_id, edge.from, edge.content});
      }
      return result;
    }

    void DeleteEdge(edge_id_t edge_id) {
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

    void DeleteNode(node_t node) {
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


    const std::unordered_set<node_t> &nodes() const {
      return nodes_;
    }

  private:
    struct Edge {
      node_t from;
      EdgeContent content;
      node_t to;
    };
    std::unordered_set<node_t> nodes_;
    std::unordered_map<edge_id_t, Edge> edges_;
    std::unordered_map<node_t, std::unordered_set<edge_id_t> > out_edges_;
    std::unordered_map<node_t, std::unordered_set<edge_id_t> > in_edges_;

    node_t next_node_;
    edge_id_t next_transition_id_;
  };

  template<typename Letter>
  struct Nfa {
    node_t initial;
    node_t final;
    std::unique_ptr<Graph<Letter>> graph;
  };
}

template<typename EdgeContent>
std::ostream &operator<<(std::ostream &o, const rbg::Graph<EdgeContent> &g) {
  std::vector<rbg::node_t> nodes(g.nodes().begin(), g.nodes().end());
  std::sort(nodes.begin(), nodes.end());
  for (rbg::node_t node : nodes) {
    o << node << " -> ";
    auto transitions = g.Transitions(node);
    std::sort(transitions.begin(), transitions.end(),
              [](const rbg::Transition<EdgeContent> &a, const rbg::Transition<EdgeContent> &b) {
                return a.to < b.to;
              });
    for (const auto &transition : transitions) {
      o << transition.to << " (edge id: " << transition.id << ", content: " << transition.content << "), ";
    }
    o << "\n";
  }
  return o;
}


#endif //RBGGAMEMANAGER_GRAPH_H
