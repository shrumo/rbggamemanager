//
// Created by shrum on 06.06.19.
//

#include <pybind11/pybind11.h>
#include <graph/graph.h>
#include <parser/parser_wrapper.h>
#include <construction/graph_creator.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>

using namespace rbg;

class Translator : public AstFunction<std::string> {
public:
  std::string GameMoveCase(const rbg_parser::game_move &move) override {
    return move.to_rbg();
  }

  std::string DefaultCase() override {
    throw std::logic_error("This isn't an action.");
  }

};

Nfa<std::string> translate(const Nfa<std::unique_ptr<Move>> &nfa) {
  Nfa<std::string> result = Nfa<std::string>();
  std::unordered_map<node_t, node_t> node_mapping;
  std::vector<node_t> sorted_nodes(nfa.graph.nodes().begin(), nfa.graph.nodes().end());
  std::sort(sorted_nodes.begin(), sorted_nodes.end());
  for (auto node : sorted_nodes) {
    node_t result_node = result.graph.AddNode();
    node_mapping[node] = result_node;
  }

  std::vector<std::string> tmp_res;
  for (const auto &edge: nfa.graph.edges()) {
    result.graph.AddEdge(node_mapping.at(edge.second.from),
                         edge.second.content ? Translator()(*edge.second.content) : "e",
                         node_mapping.at(edge.second.to));
  }

  result.initial = node_mapping.at(nfa.initial);
  result.final = node_mapping.at(nfa.final);
  return result;
}

Nfa<std::string> CreateStringNfa(const std::string &game_text) {
  auto pg = ParseGame(game_text);
  return translate(CreateGraph(*pg->get_moves()));
}

namespace py = pybind11;

PYBIND11_MODULE(rbg, m) {
  m.doc() = R"pbdoc(
        rbg module allows you to parse the games written in Regular Boardgames language
        -----------------------
        .. currentmodule:: rbg
        .. autosummary::
           :toctree: _generate
           create_nfa
    )pbdoc";

  m.def("create_nfa", &CreateStringNfa, R"pbdoc(
        Returns the nfa.
    )pbdoc");

  pybind11::class_<Graph<std::string>::Edge>(m, "Edge")
      .def_readwrite("a", &Graph<std::string>::Edge::from)
      .def_readwrite("b", &Graph<std::string>::Edge::to)
      .def_readwrite("value", &Graph<std::string>::Edge::content)
      .def("__repr__", [](const Graph<std::string>::Edge &edge) {
        return "<rbg.Edge from " + std::to_string(edge.from) + " to " + std::to_string(edge.to) + " on " +
               edge.content + ">";
      });

  pybind11::class_<Graph<std::string>>(m, "Graph")
      .def("nodes", &Graph<std::string>::nodes)
      .def("edges", &Graph<std::string>::edges);

  pybind11::class_<Nfa<std::string>>(m, "Nfa")
      .def_readwrite("graph", &Nfa<std::string>::graph)
      .def_readwrite("initial", &Nfa<std::string>::initial)
      .def_readwrite("final", &Nfa<std::string>::final);

  m.attr("__version__") = "dev";
}