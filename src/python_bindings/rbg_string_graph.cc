//
// Created by shrum on 06.06.19.
//

#include <parser/parser_wrapper.h>
#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <utility/graph.h>
#include <utility/printer.h>

#include "game_description/construction/graph_creator.h"

using namespace rbg;

Nfa<std::string> translate(const Nfa<std::unique_ptr<Move>> &nfa,
                           const Declarations &declarations) {
  Nfa<std::string> result = Nfa<std::string>();
  std::unordered_map<node_t, node_t> node_mapping;
  std::vector<node_t> sorted_nodes(nfa.graph.nodes().begin(),
                                   nfa.graph.nodes().end());
  std::sort(sorted_nodes.begin(), sorted_nodes.end());
  for (auto node : sorted_nodes) {
    node_t result_node = result.graph.NewNode();
    node_mapping[node] = result_node;
  }

  std::vector<std::string> tmp_res;
  for (const auto &edge : nfa.graph.edges_map()) {
    if (edge.second.content()->type() == MoveType::kConditionCheck) {
      auto rec_nfa = translate(
          dynamic_cast<Condition *>(edge.second.content().get())->nfa(),
          declarations);
      std::vector<node_t> rec_sorted_nodes(rec_nfa.graph.nodes().begin(),
                                           rec_nfa.graph.nodes().end());
      std::sort(rec_sorted_nodes.begin(), rec_sorted_nodes.end());
      std::unordered_map<node_t, node_t> rec_node_mapping;
      for (auto node : rec_sorted_nodes) {
        node_t result_node = result.graph.NewNode();
        rec_node_mapping[node] = result_node;
      }
      for (const auto &rec_edge : rec_nfa.graph.edges_map()) {
        result.graph.AddEdge(rec_node_mapping.at(rec_edge.second.from()),
                             rec_edge.second.content(),
                             rec_node_mapping.at(rec_edge.second.to()));
      }
      result.graph.AddEdge(node_mapping.at(edge.second.from()),
                           "ConditionCheckBegin",
                           rec_node_mapping.at(rec_nfa.initial));
      result.graph.AddEdge(rec_node_mapping.at(rec_nfa.final),
                           "ConditionCheckEnd",
                           node_mapping.at(edge.second.to()));
    } else {
      result.graph.AddEdge(
          node_mapping.at(edge.second.from()),
          MoveDescription(*edge.second.content(), declarations),
          node_mapping.at(edge.second.to()));
    }
  }

  result.initial = node_mapping.at(nfa.initial);
  result.final = node_mapping.at(nfa.final);
  return result;
}

Nfa<std::string> CreateStringNfa(const std::string &game_text) {
  auto pg = ParseGame(game_text);
  auto decl = Declarations(*pg);
  return translate(CreateNfa(*pg->get_moves(), decl, false), decl);
}

void RemoveVisitedQueries(Nfa<std::string> *nfa) {
  using rbg::edge_id_t;
  using rbg::node_t;
  std::vector<node_t> nodes_with_visited_edge;
  for (const auto &edge : nfa->graph.edges_map()) {
    if (edge.second.content() == "<VisitedQuery>") {
      nodes_with_visited_edge.push_back(edge.second.from());
    }
  }
  for (node_t node_id : nodes_with_visited_edge) {
    node_t target = nfa->graph.EdgesFrom(node_id).begin()->to();
    for (edge_id_t edge_id :
         std::vector<edge_id_t>(nfa->graph.edges_ids_to(node_id).begin(),
                                nfa->graph.edges_ids_to(node_id).end())) {
      nfa->graph.ChangeEdgeTarget(edge_id, target);
    }
    nfa->graph.EraseNode(node_id);
  }
}

Nfa<std::string> CreateStringNfaPure(const std::string &game_text) {
  auto result = CreateStringNfa(game_text);
  RemoveVisitedQueries(&result);
  return result;
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

  m.def("create_string_nfa", &CreateStringNfa, R"pbdoc(
        Returns the nfa.
    )pbdoc");

  m.def("create_string_nfa_pure", &CreateStringNfaPure,
        "Returns an nfa as would be returned by Thompsons construction. "
        "(Without VisitedQueries.)");

  pybind11::class_<Edge<std::string>>(m, "Edge")
      .def("a", &Edge<std::string>::from)
      .def("b", &Edge<std::string>::to)
      .def("value", &Edge<std::string>::content)
      .def("__repr__", [](const Edge<std::string> &edge) {
        return "<rbg.Edge from " + std::to_string(edge.from()) + " to " +
               std::to_string(edge.to()) + " on " + edge.content() + ">";
      });

  pybind11::class_<Node<std::string>>(m, "Node").def(
      "__repr__", [](const Node<std::string> &node) {
        return "<rbg.Node with id from " + std::to_string(node.id()) + ">";
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
