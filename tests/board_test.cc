//
// Created by shrum on 10.06.19.
//

#include <game_description/board.h>

#include <cassert>
#include <iostream>

using namespace rbg;

int main() {
  Board board(4, 4, 2);

  vertex_id_t v00 = board.AddVertexName("v00");

  board.initial_content().set(v00, 1);

  assert(board.vertices_count() == 4);

  board.AddEdge("v00", "right", "v10");
  board.AddEdge("v00", "down", "v01");

  vertex_id_t v10 = board.AddVertexName("v10");

  shift_edge_id left_edge_id = board.AddEdgeName("left");

  board.AddEdge(v10, left_edge_id, v00);

  assert(board.NextVertex(v00, board.AddEdgeName("right")) == v10);

  assert(board.NextVertex(v10, board.edges_names().Id("left")) == v00);
  assert(board.initial_content().at(v00) == 1);
  assert(board.initial_content().piece_count(1) == 1);

  std::cout << board << std::endl;
}