//
// Created by shrum on 21.05.19.
//

#ifndef RBGGAMEMANAGER_GRAPH_H
#define RBGGAMEMANAGER_GRAPH_H


#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ostream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <sstream>
#include <cassert>

namespace rbg {
  using node_t = uint;
  using shift_edge_id_t = uint;

  template<typename EdgeContent>
  struct Transition {
    shift_edge_id_t id;
    node_t to;
    node_t from;
    const EdgeContent &content;
  };


  template<typename EdgeContent>
  class Graph {
  public:
    struct Edge {
      node_t from;
      EdgeContent content;
      node_t to;
    };

    Graph() : next_node_(0), next_transition_id_(0) {}

    node_t AddNode() {
      node_t node = next_node_;
      next_node_++;
      nodes_.insert(node);
      out_edges_[node];
      in_edges_[node];
      return node;
    }

    shift_edge_id_t AddEdge(node_t from, EdgeContent content, node_t to) {
      assert(nodes_.find(from) != nodes_.end() && "Node doesn't exist.");
      assert(nodes_.find(to) != nodes_.end() && "Node doesn't exist.");
      shift_edge_id_t transition_id = next_transition_id_;
      next_transition_id_++;
      edges_[transition_id].from = from;
      edges_[transition_id].content = std::move(content);
      edges_[transition_id].to = to;
      out_edges_[from].insert(transition_id);
      in_edges_[to].insert(transition_id);
      return transition_id;
    }

    std::vector<Transition<EdgeContent>> Transitions(node_t node) const {
      assert(nodes_.find(node) != nodes_.end() && "Node doesn't exist.");
      std::vector<Transition<EdgeContent>> result;
      for (shift_edge_id_t edge_id : out_edges_.at(node)) {
        const Edge &edge = edges_.at(edge_id);
        result.push_back(Transition<EdgeContent>{edge_id, edge.to, node, edge.content});
      }
      return result;
    }


    std::vector<Transition<EdgeContent>> InTransitions(node_t node) const {
      assert(nodes_.find(node) != nodes_.end() && "Node doesn't exist.");
      std::vector<Transition<EdgeContent>> result;
      for (shift_edge_id_t edge_id : in_edges_.at(node)) {
        const Edge &edge = edges_.at(edge_id);
        result.push_back(Transition<EdgeContent>{edge_id, node, edge.from, edge.content});
      }
      return result;
    }

    void DeleteEdge(shift_edge_id_t edge_id) {
      assert(edges_.find(edge_id) != edges_.end() && "Edge doesn't exist.");
      {
        const Edge &edge = edges_[edge_id];
        out_edges_[edge.from].erase(edge_id);
        in_edges_[edge.to].erase(edge_id);
      }
      edges_.erase(edge_id);
    }

    void DeleteNode(node_t node) {
      assert(nodes_.find(node) != nodes_.end() && "Node doesn't exist.");
      std::unordered_set<shift_edge_id_t> edges_to_delete;
      edges_to_delete.insert(out_edges_[node].begin(), out_edges_[node].end());
      edges_to_delete.insert(in_edges_[node].begin(), in_edges_[node].end());
      for (shift_edge_id_t edge_id : edges_to_delete) {
        DeleteEdge(edge_id);
      }
      nodes_.erase(node);
      out_edges_.erase(node);
      in_edges_.erase(node);
    }

    node_t IdentifyNodes(node_t a, node_t b) {
      for (const auto &transition : Transitions(b)) {
        out_edges_[b].erase(transition.id);
        out_edges_[a].insert(transition.id);
        edges_[transition.id].from = a;
      }

      for (const auto &transition : InTransitions(b)) {
        in_edges_[b].erase(transition.id);
        in_edges_[a].insert(transition.id);
        edges_[transition.id].to = a;
      }
      DeleteNode(b);
      return a;
    }

    const std::unordered_set<node_t> &nodes() const {
      return nodes_;
    }

    EdgeContent &edge_content(shift_edge_id_t edge) {
      return edges_[edge].content;
    }

    const std::unordered_map<shift_edge_id_t, Edge> &edges() const {
      return edges_;
    }

    void ChangeTransitionTo(shift_edge_id_t edge_id, node_t target) {
      assert(edges_.find(edge_id) != edges_.end() && "Edge doesn't exist.");
      assert(nodes_.find(target) != nodes_.end() && "Node doesn't exist.");
      auto &edge = edges_[edge_id];
      in_edges_[edge.to].erase(edge_id);
      edge.to = target;
      in_edges_[target].insert(edge_id);
    }

  private:
    std::unordered_set<node_t> nodes_;
    std::unordered_map<shift_edge_id_t, Edge> edges_;
    std::unordered_map<node_t, std::unordered_set<shift_edge_id_t> > out_edges_;
    std::unordered_map<node_t, std::unordered_set<shift_edge_id_t> > in_edges_;

    node_t next_node_;
    shift_edge_id_t next_transition_id_;
  };

  template<typename Letter>
  struct Nfa {
    Graph<Letter> graph;
    node_t initial;
    node_t final;
  };
}

template<typename EdgeContent, typename ContentPrinter>
std::string GraphDescription(const rbg::Graph<EdgeContent> &g, ContentPrinter printer) {
  std::stringstream result;
  std::vector<rbg::node_t> nodes(g.nodes().begin(), g.nodes().end());
  std::sort(nodes.begin(), nodes.end());
  bool first_sep = false;
  for (rbg::node_t node : nodes) {
    if (!first_sep) {
      first_sep = true;
    } else {
      result << "\n";
    }
    result << node << " -> ";
    auto transitions = g.Transitions(node);
    std::vector<size_t> indices;
    indices.reserve(transitions.size());
    for (size_t i = 0; i < transitions.size(); i++) {
      indices.push_back(i);
    }
    std::sort(indices.begin(), indices.end(),
              [&](const int &a_ind, const int &b_ind) {
                const rbg::Transition<EdgeContent> &a = transitions[a_ind];
                const rbg::Transition<EdgeContent> &b = transitions[b_ind];
                return a.to < b.to || (a.to == b.to && a.id < b.id);
              });
    for (size_t i = 0; i < transitions.size(); i++) {
      if (i != 0) {
        result << ", ";
      }
      const auto &transition = transitions[indices[i]];
      result << transition.to << " (edge id: " << transition.id << ", content: " << printer(transition.content)
             << ")";
    }
  }
  return result.str();
}


#endif //RBGGAMEMANAGER_GRAPH_H
