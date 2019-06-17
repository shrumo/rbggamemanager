//
// Created by shrum on 09.06.19.
//

#ifndef RBGGAMEMANAGER_BOARD_H
#define RBGGAMEMANAGER_BOARD_H

#include "namesresolver.h"
#include <ostream>

namespace rbg {
  using vertex_id_t = name_id_t;
  using shift_edge_id_t = name_id_t;

  class Board {
  public:
    Board(uint vertices_count, uint edges_count) :
        fields_(vertices_count),
        neighbours_(vertices_count, std::vector<vertex_id_t>(edges_count, vertices_count)),
        edges_count_(edges_count) {}

    void AddEdge(const std::string &a, const std::string &edge, const std::string &b) {
      vertex_id_t a_vertex = AddVertexName(a);
      vertex_id_t b_vertex = AddVertexName(b);
      shift_edge_id_t edge_id = AddEdgeName(edge);
      AddEdge(a_vertex, edge_id, b_vertex);
    }

    void AddEdge(vertex_id_t a_vertex, shift_edge_id_t edge_id, vertex_id_t b_vertex) {
      neighbours_[a_vertex][edge_id] = b_vertex;
    }

    name_id_t &operator[](vertex_id_t vertex) {
      return fields_[vertex];
    }

    name_id_t operator[](vertex_id_t vertex) const {
      return fields_[vertex];
    }

    size_t vertices_count() const {
      return fields_.size();
    }

    size_t edges_count() const {
      return edges_count_;
    }

    vertex_id_t AddVertexName(const std::string &v) {
      return vertices_resolver_.Id(v);
    }

    vertex_id_t AddEdgeName(const std::string &e) {
      return edges_resolver_.Id(e);
    }

    vertex_id_t NextVertex(vertex_id_t current, shift_edge_id_t edge) const {
      return neighbours_[current][edge];
    }

    const NamesResolver &edges_names() const {
      return edges_resolver_;
    }

    const NamesResolver &vertices_names() const {
      return vertices_resolver_;
    }

  private:
    NamesResolver vertices_resolver_;
    NamesResolver edges_resolver_;
    std::vector<name_id_t> fields_;
    std::vector<std::vector<vertex_id_t>> neighbours_;
    uint edges_count_;
  };
}


std::ostream &operator<<(std::ostream &o, const rbg::Board &res);

#endif //RBGGAMEMANAGER_BOARD_H
