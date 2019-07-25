//
// Created by shrum on 16.06.19.
//

#include <parser/parser_wrapper.h>
#include <cassert>
#include <queue>
#include <unordered_set>
#include <game_description/construction/declarations_creator.h>

using namespace rbg;
using namespace std;

const char *kSmallGame = R"LIM(
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [e,r]
    [e,b])
#rules = ->red
)LIM";

int main() {
  auto pg = ParseGame(kSmallGame);
  auto result = CreateDeclarations(*pg);
  assert(result.pieces_resolver.size() == 3);

  assert(result.pieces_resolver.contains("e"));
  assert(result.pieces_resolver.contains("r"));
  assert(result.pieces_resolver.contains("b"));

  assert(result.players_resolver.size() == 2 + 1);
  assert(result.players_resolver.contains("red"));
  assert(result.players_resolver.contains("blue"));

  assert(result.variables_resolver.size() == 3);
  assert(result.variables_resolver.contains("red"));
  assert(result.variables_resolver.contains("blue"));
  assert(result.variables_resolver.contains("turn"));

  assert(result.variables_bounds[result.variables_resolver.Id("red")] == 100);
  assert(result.variables_bounds[result.variables_resolver.Id("blue")] == 50);
  assert(result.variables_bounds[result.variables_resolver.Id("turn")] == 10);

  assert(result.initial_board.vertices_count() == 5);
  assert(result.initial_board.edges_count() == 4);

  std::unordered_set<std::string> square_edges;
  for (vertex_id_t id = 1; id < result.initial_board.vertices_count(); id++) {
    if (result.initial_board.NextVertex(id, result.initial_board.edges_names().Id("right")) ==
        result.initial_board.vertices_count() &&
        result.initial_board.NextVertex(id, result.initial_board.edges_names().Id("up")) ==
        result.initial_board.vertices_count()) {
      assert(result.initial_board.initial_content().at(id) == result.pieces_resolver.Id("r"));
      square_edges.insert("(right,up)");
    } else if (result.initial_board.NextVertex(id, result.initial_board.edges_names().Id("left")) ==
               result.initial_board.vertices_count() &&
               result.initial_board.NextVertex(id, result.initial_board.edges_names().Id("up")) ==
               result.initial_board.vertices_count()) {
      assert(result.initial_board.initial_content().at(id) == result.pieces_resolver.Id("e"));
      square_edges.insert("(left,up)");
    } else if (result.initial_board.NextVertex(id, result.initial_board.edges_names().Id("right")) ==
               result.initial_board.vertices_count() &&
               result.initial_board.NextVertex(id, result.initial_board.edges_names().Id("down")) ==
               result.initial_board.vertices_count()) {
      assert(result.initial_board.initial_content().at(id) == result.pieces_resolver.Id("b"));
      square_edges.insert("(right,down)");
    } else if (result.initial_board.NextVertex(id, result.initial_board.edges_names().Id("left")) ==
               result.initial_board.vertices_count() &&
               result.initial_board.NextVertex(id, result.initial_board.edges_names().Id("down")) ==
               result.initial_board.vertices_count()) {
      assert(result.initial_board.initial_content().at(id) == result.pieces_resolver.Id("e"));
      square_edges.insert("(left,down)");
    }
  }
  assert(square_edges.size() == 4);

  std::cout << "Pieces resolver:\n";
  std::cout << result.pieces_resolver << std::endl;

  std::cout << "Players resolver:\n";
  std::cout << result.players_resolver << std::endl;

  std::cout << "Variables resolver:\n";
  std::cout << result.variables_resolver << std::endl;

  std::cout << "Board:\n";
  std::cout << result.initial_board << std::endl;
}