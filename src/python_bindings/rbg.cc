//
// Created by shrum on 06.06.19.
//

#include <game_state/construction/game_state_creator.h>
#include <game_state/game_state.h>
#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "networking/client.h"
#include "utility/calculate_sim.h"

using namespace rbg;

namespace py = pybind11;

PYBIND11_MODULE(rbg, m) {
  m.doc() = R"pbdoc(
        rbg module allows you to create a simple rbg server client
        -----------------------
        .. currentmodule:: client
        .. autosummary::
           :toctree: _generate
           create_nfa
    )pbdoc";

  pybind11::class_<Client>(m, "Client")
      .def(py::init<const std::string &, const std::string &>())
      .def("Write", &Client::Write)
      .def("description", &Client::description)
      .def("player", &Client::player)
      .def("Ready", &Client::Ready)
      .def("Read", &Client::Read)
      .def("ReadDeadline", &Client::ReadDeadline)
      .def("ReadReset", &Client::ReadReset)
      .def("preparation_time", &Client::preparation_time);

  pybind11::class_<MoveRevertInformation>(m, "MoveRevertInformation");

  pybind11::class_<ModifierApplication>(m, "ModifierApplication")
      .def_readonly("vertex", &ModifierApplication::vertex)
      .def_readonly("modifier_index", &ModifierApplication::modifier_index);

  pybind11::class_<GameState>(m, "GameState")
      .def("Moves", &GameState::Moves)
      .def("Apply", &GameState::Apply)
      .def("Revert", &GameState::Revert)
      .def("Reset", &GameState::Reset)
      .def("declarations", &GameState::declarations)
      .def("board_content", &GameState::board_content)
      .def("current_pos", &GameState::current_pos)
      .def("current_player", &GameState::current_player)
      .def("variables_values", &GameState::variables_values);

  pybind11::class_<Declarations>(m, "Declarations")
      .def("pieces_resolver", &Declarations::pieces_resolver)
      .def("variables_resolver", &Declarations::variables_resolver)
      .def("players_resolver", &Declarations::players_resolver)
      .def("variable_bound", &Declarations::variable_bound)
      .def("initial_board", &Declarations::initial_board)
      .def("keeper_id", &Declarations::keeper_id);

  pybind11::class_<NamesResolver>(m, "NamesResolver")
      .def("Id", &NamesResolver::Id)
      .def("Name", &NamesResolver::Name)
      .def("size", &NamesResolver::size);

  pybind11::class_<BoardContent>(m, "BoardContent")
      .def("at", &BoardContent::at)
      .def("vertices_count", &BoardContent::vertices_count)
      .def("piece_count", &BoardContent::piece_count);

  pybind11::class_<Board>(m, "Board")
      .def("vertices_count", &Board::vertices_count)
      .def("edges_count", &Board::edges_count)
      .def("edges_names", &Board::edges_names)
      .def("vertices_names", &Board::vertices_names)
      .def("NextVertex", &Board::NextVertex);

  m.def("CreateGameState", &CreateGameState,
        R"pbdoc(Returns the GameState object.)pbdoc");

  m.def("RandomSimulations", &RandomSimulations,
        R"pbdoc(Runs random simulations for an initial game state.)pbdoc");

  m.attr("__version__") = "dev";
}
