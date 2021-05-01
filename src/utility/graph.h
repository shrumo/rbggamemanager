//
// Created by shrum on 21.05.19.
//

#ifndef RBGGAMEMANAGER_GRAPH_H
#define RBGGAMEMANAGER_GRAPH_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <memory>
#include <ostream>
#include <sstream>
#include <stl_extension/iterator_transform.h>
#include <stl_extension/range.h>
#include <stl_extension/utility.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace rbg {
using node_t = uint;
using edge_id_t = uint;

// Returns nodes ids.
// Every subsequent call to this function will give different value.
static inline node_t new_node_id() {
  static node_t next_node_id_ = 0;
  return next_node_id_++;
}

// Return edges ids.
// Every subsequent call to this function will give different value.
static inline edge_id_t new_edge_id() {
  static edge_id_t next_edge_id_ = 0;
  return next_edge_id_++;
}

template <typename EdgeContent> class Node;

template <typename EdgeContent> class Graph;

template <typename EdgeContent> class Edge;

template <typename EdgeContent> class Node {
public:
  const std::list<edge_id_t> &out_edges_ids() const {
    return out_edges_.edges();
  }

  const std::unordered_set<edge_id_t> &in_edges_ids() const {
    return in_edges_;
  }

  node_t id() const { return id_; }

private:
  explicit Node(node_t id) : id_(id) {}

  node_t id_;

  class EdgesSequence {
  public:
    void Erase(edge_id_t edge_id) {
      edges_.erase(edges_positions_[edge_id]);
      edges_positions_.erase(edge_id);
    }
    void PushFront(edge_id_t edge_id) {
      edges_.push_front(edge_id);
      edges_positions_[edge_id] = edges_.begin();
    }

    void PushBack(edge_id_t edge_id) {
      edges_.push_back(edge_id);
      auto last = edges_.end();
      --last;
      edges_positions_[edge_id] = last;
    }

    const std::list<edge_id_t> &edges() const { return edges_; }

  private:
    std::unordered_map<edge_id_t, std::list<edge_id_t>::iterator>
        edges_positions_;
    std::list<edge_id_t> edges_;
  } out_edges_;

  std::unordered_set<edge_id_t> in_edges_;

  friend class Graph<EdgeContent>;
};

template <typename EdgeContent> class Edge {
public:
  node_t to() const { return to_; }

  node_t from() const { return from_; }

  const EdgeContent &content() const { return content_; }

  EdgeContent &modifiable_content() { return content_; }

  edge_id_t id() const { return id_; }

private:
  Edge(edge_id_t id, node_t from, EdgeContent content, node_t to)
      : id_(id), to_(to), from_(from), content_(std::move(content)) {}

  edge_id_t id_;
  node_t to_;
  node_t from_;
  EdgeContent content_;

  friend class Graph<EdgeContent>;
};

template <typename EdgeContent> class Graph {
public:
  node_t NewNode() {
    node_t node_id = new_node_id();
    nodes_.emplace(node_id, Node<EdgeContent>(node_id));
    return node_id;
  }

  edge_id_t AddEdge(node_t from, EdgeContent content, node_t to) {
    edge_id_t edge_id = new_edge_id();
    edges_.insert(std::pair<edge_id_t, Edge<EdgeContent>>{
        edge_id, Edge<EdgeContent>(edge_id, from, std::move(content), to)});
    nodes_.at(from).out_edges_.PushBack(edge_id);
    nodes_.at(to).in_edges_.insert(edge_id);
    return edge_id;
  }

  Edge<EdgeContent> &GetEdge(edge_id_t edge_id) { return edges_.at(edge_id); }

  const Edge<EdgeContent> &GetEdge(edge_id_t edge_id) const {
    return edges_.at(edge_id);
  }

  const std::list<edge_id_t> &edges_ids_from(node_t node_id) const {
    return GetNode(node_id).out_edges_ids();
  }

  decltype(auto) EdgesFrom(node_t node_id) const {
    auto extract_edge =
        [this](const edge_id_t &edge_id) -> const Edge<EdgeContent> & {
      return GetEdge(edge_id);
    };
    return std_ext::range<std_ext::transform_iterator<
        typename std::list<edge_id_t>::const_iterator, decltype(extract_edge)>>(
        std_ext::transform_iterator<
            typename std::list<edge_id_t>::const_iterator,
            decltype(extract_edge)>(edges_ids_from(node_id).begin(),
                                    extract_edge),
        std_ext::transform_iterator<
            typename std::list<edge_id_t>::const_iterator,
            decltype(extract_edge)>(edges_ids_from(node_id).end(),
                                    extract_edge),
        edges_ids_from(node_id).size());
  }

  const std::unordered_set<edge_id_t> &edges_ids_to(node_t node_id) const {
    return GetNode(node_id).in_edges_ids();
  }

  decltype(auto) EdgesTo(node_t node_id) {
    auto extract_edge =
        [this](const edge_id_t &edge_id) -> Edge<EdgeContent> & {
      return GetEdge(edge_id);
    };
    return std_ext::range<std_ext::transform_iterator<
        typename std::unordered_set<edge_id_t>::const_iterator,
        decltype(extract_edge)>>(
        std_ext::transform_iterator<
            typename std::unordered_set<edge_id_t>::const_iterator,
            decltype(extract_edge)>(edges_ids_to(node_id).begin(),
                                    extract_edge),
        std_ext::transform_iterator<
            typename std::unordered_set<edge_id_t>::const_iterator,
            decltype(extract_edge)>(edges_ids_to(node_id).end(), extract_edge),
        edges_ids_to(node_id).size());
  }

  decltype(auto) EdgesTo(node_t node_id) const {
    auto extract_edge =
        [this](const edge_id_t &edge_id) -> const Edge<EdgeContent> & {
      return GetEdge(edge_id);
    };
    return std_ext::range<std_ext::transform_iterator<
        typename std::unordered_set<edge_id_t>::const_iterator,
        decltype(extract_edge)>>(
        std_ext::transform_iterator<
            typename std::unordered_set<edge_id_t>::const_iterator,
            decltype(extract_edge)>(edges_ids_to(node_id).begin(),
                                    extract_edge),
        std_ext::transform_iterator<
            typename std::unordered_set<edge_id_t>::const_iterator,
            decltype(extract_edge)>(edges_ids_to(node_id).end(), extract_edge),
        edges_ids_to(node_id).size());
  }

  void EraseNode(node_t node_id) {
    auto from_ids_to_erase = edges_ids_from(node_id);
    for (edge_id_t edge_id : from_ids_to_erase) {
      EraseEdge(edge_id);
    }
    auto to_ids_to_erase = edges_ids_to(node_id);
    for (edge_id_t edge_id : to_ids_to_erase) {
      EraseEdge(edge_id);
    }
    nodes_.erase(node_id);
  }

  void EraseEdge(edge_id_t edge_id) {
    auto &edge = GetEdge(edge_id);
    auto &node_to = GetNode(edge.to());
    node_to.in_edges_.erase(edge_id);
    auto &node_from = GetNode(edge.from());
    node_from.out_edges_.Erase(edge_id);
    edges_.erase(edge_id);
  }

  void MergeWithNode(node_t a, node_t b) {
    auto ids_from = edges_ids_from(b);
    for (edge_id_t edge_id : ids_from) {
      ChangeEdgeSource(edge_id, a);
    }
    auto ids_to = edges_ids_to(b);
    for (edge_id_t edge_id : ids_to) {
      ChangeEdgeTarget(edge_id, a);
    }
    EraseNode(b);
  }

  using NodesRange = std_ext::range<std_ext::transform_iterator<
      typename std::unordered_map<node_t, Node<EdgeContent>>::const_iterator,
      std_ext::first_of_pair<const node_t, Node<EdgeContent>>>>;

  NodesRange nodes() const {
    return NodesRange(nodes_.begin(), nodes_.end(), nodes_.size());
  }

  const std::unordered_map<node_t, Node<EdgeContent>> &nodes_map() const {
    return nodes_;
  }

  const std::unordered_map<edge_id_t, Edge<EdgeContent>> &edges_map() const {
    return edges_;
  }

  Node<EdgeContent> &GetNode(node_t node_id) { return nodes_.at(node_id); }

  const Node<EdgeContent> &GetNode(node_t node_id) const {
    return nodes_.at(node_id);
  }

  void ChangeEdgeSource(edge_id_t edge_id, node_t new_source) {
    auto &edge = GetEdge(edge_id);
    GetNode(edge.from()).out_edges_.Erase(edge_id);
    edge.from_ = new_source;
    GetNode(edge.from()).out_edges_.PushBack(edge_id);
  }

  void ChangeEdgeTarget(edge_id_t edge_id, node_t new_target) {
    auto &edge = GetEdge(edge_id);
    GetNode(edge.to()).in_edges_.erase(edge_id);
    edge.to_ = new_target;
    GetNode(edge.to()).in_edges_.insert(edge_id);
  }

private:
  std::unordered_map<node_t, Node<EdgeContent>> nodes_;
  std::unordered_map<edge_id_t, Edge<EdgeContent>> edges_;
};

template <typename Letter> struct Nfa {
  Graph<Letter> graph;
  node_t initial;
  node_t final;
};
} // namespace rbg

template <typename EdgeContent, typename ContentPrinter>
std::string GraphDescription(const rbg::Graph<EdgeContent> &g,
                             ContentPrinter printer) {
  std::stringstream result;
  std::vector<rbg::node_t> nodes;
  nodes.reserve(g.nodes_map().size());
  for (const auto &pair : g.nodes_map()) {
    nodes.push_back(pair.first);
  }
  std::sort(nodes.begin(), nodes.end());
  bool first_sep = false;
  for (const auto &node : nodes) {
    if (!first_sep) {
      first_sep = true;
    } else {
      result << "\n";
    }
    result << node << " -> ";
    auto edges = g.EdgesFrom(node);
    bool comma_skipped = false;
    for (const auto &edge : edges) {
      if (!comma_skipped) {
        comma_skipped = true;
      } else {
        result << ", ";
      }
      result << edge.to() << " (edge id: " << edge.id()
             << ", content: " << printer(edge.content()) << ")";
    }
  }
  return result.str();
}

#endif // RBGGAMEMANAGER_GRAPH_H
