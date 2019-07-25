//
// Created by shrum on 17.06.19.
//

#include "board.h"

std::ostream &operator<<(std::ostream &o, const rbg::Board &res) {
  o << "<Board with mapping {";
  for (rbg::vertex_id_t i = 0; i < res.vertices_count(); i++) {
    if (i != 0) {
      o << "; ";
    }
    o << (i < res.vertices_names().size() ? res.vertices_names().Name(i) : "<vertex>") << "(" << i << "): ";
    for (rbg::shift_edge_id j = 0; j < res.edges_count(); j++) {
      if (j != 0) {
        o << ", ";
      }
      rbg::vertex_id_t next = res.NextVertex(i, j);
      o << (j < res.edges_names().size() ? res.edges_names().Name(j) : "<edge>") << "(" << j << ")-> "
        << (next != res.vertices_count() ? std::to_string(next) : "<none>");
    }
  }
  o << ">";
  return o;
}
