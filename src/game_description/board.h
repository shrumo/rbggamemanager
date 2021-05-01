//
// Created by shrum on 09.06.19.
//

#ifndef RBGGAMEMANAGER_BOARD_H
#define RBGGAMEMANAGER_BOARD_H

#include <ostream>

#include "names_resolver.h"

namespace rbg {
using vertex_id_t = name_id_t;
using shift_edge_id = name_id_t;
using piece_id_t = name_id_t;

class BoardContent {
public:
  BoardContent(uint vertices_count, uint piece_count)
      : fields_(vertices_count, 0), pieces_counts_(piece_count) {
    // The piece on the <out> field (index 0) shouldn't be counted
    pieces_counts_[0] = vertices_count - 1;
  }

  piece_id_t at(vertex_id_t vertex) const { return fields_[vertex]; }

  void set(vertex_id_t vertex, piece_id_t piece) {
    pieces_counts_[at(vertex)] -= 1;
    fields_[vertex] = piece;
    pieces_counts_[piece] += 1;
  }

  size_t vertices_count() const { return fields_.size(); }

  uint piece_count(piece_id_t piece) const { return pieces_counts_[piece]; }

private:
  std::vector<piece_id_t> fields_;
  std::vector<uint> pieces_counts_;
};

class Board {
public:
  Board(uint vertices_count, uint edges_count, uint piece_count)
      : neighbours_(vertices_count,
                    std::vector<vertex_id_t>(edges_count, vertices_count)),
        edges_count_(edges_count),
        initial_content_(vertices_count, piece_count) {}

  void AddEdge(const std::string &a, const std::string &edge,
               const std::string &b) {
    vertex_id_t a_vertex = AddVertexName(a);
    vertex_id_t b_vertex = AddVertexName(b);
    shift_edge_id edge_id = AddEdgeName(edge);
    AddEdge(a_vertex, edge_id, b_vertex);
  }

  void AddEdge(vertex_id_t a_vertex, shift_edge_id edge_id,
               vertex_id_t b_vertex) {
    neighbours_[a_vertex][edge_id] = b_vertex;
  }

  size_t vertices_count() const { return initial_content_.vertices_count(); }

  size_t edges_count() const { return edges_count_; }

  vertex_id_t AddVertexName(const std::string &v) {
    return vertices_resolver_.AddName(v);
  }

  vertex_id_t AddEdgeName(const std::string &e) {
    return edges_resolver_.AddName(e);
  }

  vertex_id_t NextVertex(vertex_id_t current, shift_edge_id edge) const {
    return neighbours_[current][edge];
  }

  const NamesResolver &edges_names() const { return edges_resolver_; }

  const NamesResolver &vertices_names() const { return vertices_resolver_; }

  const BoardContent &initial_content() const { return initial_content_; }

  BoardContent &initial_content() { return initial_content_; }

private:
  NamesResolver vertices_resolver_;
  NamesResolver edges_resolver_;
  std::vector<std::vector<vertex_id_t>> neighbours_;
  uint edges_count_;
  BoardContent initial_content_;
};
} // namespace rbg

std::ostream &operator<<(std::ostream &o, const rbg::Board &res);

#endif // RBGGAMEMANAGER_BOARD_H
