//
// Created by shrum on 06.06.19.
//

#include <pybind11/pybind11.h>
#include <utility/graph.h>
#include <parser/parser_wrapper.h>
#include "game_description/construction/graph_creator.h"
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <utility/printer.h>


using namespace rbg;

Nfa<std::string> translate(const Nfa<std::unique_ptr<Move>> &nfa, const Declarations &declarations) {
  Nfa<std::string> result = Nfa<std::string>();
  std::unordered_map<node_t, node_t> node_mapping;
  std::vector<node_t> sorted_nodes(nfa.graph.nodes().begin(), nfa.graph.nodes().end());
  std::sort(sorted_nodes.begin(), sorted_nodes.end());
  for (auto node : sorted_nodes) {
    node_t result_node = result.graph.NewNode();
    node_mapping[node] = result_node;
  }

  std::vector<std::string> tmp_res;
  for (const auto &edge: nfa.graph.edges_map()) {
    result.graph.AddEdge(node_mapping.at(edge.second.from()),
                         MoveDescription(*edge.second.content(), declarations),
                         node_mapping.at(edge.second.to()));
  }

  result.initial = node_mapping.at(nfa.initial);
  result.final = node_mapping.at(nfa.final);
  return result;
}

Nfa<std::string> CreateStringNfa(const std::string &game_text) {
  auto pg = ParseGame(game_text);
  auto decl = Declarations(*pg);
  return translate(CreateNfa(*pg->get_moves(), decl), decl);
}

namespace py = pybind11;

PYBIND11_MODULE(rbg_string_graph, m) {
  m.doc() = R"pbdoc(
        rbg_string_graph module allows you to parse the games written in Regular Boardgames language and show them as a graph. For playing purposes please use the rbg module. (This is only a simplified version.)
        -----------------------
        .. currentmodule:: rbg_string_graph
        .. autosummary::
           :toctree: _generate
           create_nfa
    )pbdoc";

  m.def("create_nfa", &CreateStringNfa, R"pbdoc(
        Returns the nfa.
    )pbdoc");

  pybind11::class_<Edge<std::string>>(m, "Edge")
      .def("a", &Edge<std::string>::from)
      .def("b", &Edge<std::string>::to)
      .def("value", &Edge<std::string>::content)
      .def("__repr__", [](const Edge<std::string> &edge) {
        return "<rbg.Edge from " + std::to_string(edge.from()) + " to " + std::to_string(edge.to()) + " on " +
               edge.content() + ">";
      });

  pybind11::class_<Node<std::string>>(m, "Node")
      .def("__repr__", [](const Node<std::string> &node) {
        return "<rbg.Node with id from " + std::to_string(node.id()) +">";
      });

  pybind11::class_<Graph<std::string>>(m, "Graph")
      .def("nodes", &Graph<std::string>::nodes_map)
      .def("edges", &Graph<std::string>::edges_map);

  pybind11::class_<Nfa<std::string>>(m, "Nfa")
      .def_readwrite("graph", &Nfa<std::string>::graph)
      .def_readwrite("initial", &Nfa<std::string>::initial)
      .def_readwrite("final", &Nfa<std::string>::final);

  m.attr("__version__") = "dev";
}
