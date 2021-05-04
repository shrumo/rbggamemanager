//
// Created by shrum on 23.12.20.
//
#include <game_description/construction/graph_creator.h>
#include <parser/parser_wrapper.h>
#include <utility/printer.h>

#include <cassert>
#include <queue>

using namespace rbg;
using namespace std;

const char *kSmallGame = R"LIM(
#players = red(100), blue(100)
#pieces = e
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [e,e]
    [e,e])
//     A     B                     
#rules= ->red ( up + down )* [$ turn = turn+1] (up + right + left)* down ->blue
)LIM";

int main() {
  auto pg = ParseGame(kSmallGame);
  auto declarations = Declarations(*pg);
  auto result = CreateNfa(*pg->get_moves(), declarations);
  NfaBoardProduct board_product(result, declarations.initial_board(),
                                {/* vertex_id= */ 1});
  cout << "Nfa board product created starting with vertex: "
       << declarations.initial_board().vertices_names().Name(/* vertex_id= */ 1)
       << endl;
  node_t initial = board_product.node(
      /* vertex_id= */ 1,
      result.initial);  // Corresponding to place A in initial vertex (left up)
  cout << "Initial node in NfaBoardGraph: " << initial << endl;
  cout << "Initial and final node in original graph: " << result.initial << " "
       << result.final << endl;

  assert(board_product.edges_ids_from(initial).size() == 1);
  assert(
      result.graph
          .GetEdge(board_product
                       .GetEdge(board_product.edges_ids_from(initial).front())
                       .content())
          .content()
          ->type() == MoveType::kPlayerSwitchType);

  cout << "Original graph has " << result.graph.nodes().size() << " nodes."
       << endl;
  cout << "Board graph has " << board_product.nodes().size() << " nodes."
       << endl;

  unordered_set<node_t> originals_visited;
  queue<node_t> originals_to_visit;
  originals_to_visit.push(result.initial);
  originals_visited.insert(result.initial);
  while (!originals_to_visit.empty()) {
    node_t node = originals_to_visit.front();
    originals_to_visit.pop();
    unordered_set<edge_id_t> original_outs(
        result.graph.edges_ids_from(node).begin(),
        result.graph.edges_ids_from(node).end());

    cout << "Original node " << node << endl;
    cout << "Original out edges: " << endl;
    for (auto &edge : result.graph.EdgesFrom(node)) {
      cout << MoveDescription(*edge.content(), declarations) << " to "
           << edge.to() << endl;

      if (originals_visited.find(edge.to()) == originals_visited.end()) {
        originals_to_visit.push(edge.to());
        originals_visited.insert(edge.to());
      }
    }
    cout << "Correponding board graph nodes: " << endl;
    for (vertex_id_t vertex = 1;
         vertex < declarations.initial_board().vertices_count(); vertex++) {
      if (board_product.vertex_node_exists(vertex, node)) {
        node_t board_product_vertex = board_product.node(vertex, node);
        cout << "\t" << board_product_vertex << " ("
             << declarations.initial_board().vertices_names().Name(vertex)
             << ", " << node << ")\n";
        cout << "\tWith outs:" << endl;
        for (auto &edge : board_product.EdgesFrom(board_product_vertex)) {
          assert(original_outs.find(edge.content()) != original_outs.end());
          auto vertex_node_to = board_product.vertex_node(edge.to());
          cout << "\t\t"
               << MoveDescription(
                      *result.graph.GetEdge(edge.content()).content(),
                      declarations)
               << " to " << edge.to() << " ("
               << declarations.initial_board().vertices_names().Name(
                      vertex_node_to.first)
               << ", " << vertex_node_to.second << ") " << endl;
        }
      } else {
        cout << "\tnone (" << vertex << ", " << node << ")\n";
      }
    }
    cout << endl;
  }
}