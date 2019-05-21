//
// Created by shrum on 21.05.19.
//

#ifndef RBGGAMEMANAGER_GRAPH_H
#define RBGGAMEMANAGER_GRAPH_H


#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <actions/action.h>

namespace graph {
  using node_t = std::size_t;
  using edge_id_t = std::size_t;

  struct Transition {
    edge_id_t id;
    node_t node;
    const Action *action;
  };

  class Graph {
  public:
    Graph() : next_node_(0), next_transition_id_(0) {}

    node_t AddNode();

    edge_id_t AddEdge(node_t from, const Action *action, node_t to);

    std::vector<Transition> Transitions(node_t node) const;

    std::vector<Transition> InTransitions(node_t node) const;

    void DeleteEdge(edge_id_t edge_id);

    void DeleteNode(node_t node);

    const std::unordered_set<node_t> &nodes() const {
      return nodes_;
    }

  private:
    struct Edge {
      node_t from;
      const Action *action;
      node_t to;
    };
    std::unordered_set<node_t> nodes_;
    std::unordered_map<edge_id_t, Edge> edges_;
    std::unordered_map<node_t, std::unordered_set<edge_id_t> > out_edges_;
    std::unordered_map<node_t, std::unordered_set<edge_id_t> > in_edges_;

    node_t next_node_;
    edge_id_t next_transition_id_;
  };
}

std::ostream &operator<<(std::ostream &o, const graph::Graph &g);

#endif //RBGGAMEMANAGER_GRAPH_H
