//
// Created by shrum on 02.07.19.
//

#include <iostream>
#include <memory>
#include <vector>

#include <networking/client.h>
#include <rbgParser/src/game_items.hpp>
#include <game_state/construction/game_state_creator.h>
#include <utility/printer.h>

using namespace rbg;

int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 4) {
    std::cerr << "Usage " << argv[0] << " <host> <port> <seed>\n";
    return 1;
  }

  if(argc == 4) {
    srand(atoi(argv[3]));
  } else {
    srand(static_cast<unsigned int>(time(nullptr)));
  }

  rbg_parser::messages_container msg;
  Client client(argv[1], argv[2]);

  GameState state = CreateGameState(client.description());
  auto actions_translator = ActionsDescriptionsMap(client.description());

  player_id_t assigned_player = client.player();

  std::cout << "I am player: " << state.declarations().players_resolver().Name(assigned_player) << " (" << assigned_player << ")"
            << std::endl;

  auto moves = state.Moves();
  while (!moves.empty()) {
    if (state.current_player() == assigned_player) {
      std::cout << "\n";
      std::cout << RectangularBoardDescription(state.board_content(), state.declarations()) << std::endl;
      std::cout << "Variables values are:" << std::endl;
      std::cout << VariablesValuesDescription(state) << std::endl;
      auto move = moves[rand() % moves.size()];
      state.Apply(move);
      std::cout << "Sending move:" << std::endl;
      for(const auto& mod : move) {
        std::cout << "\t" << state.declarations().initial_board().vertices_names().Name(mod.vertex) << " (" << mod.vertex << ") "
           << actions_translator[mod.modifier_index] << " (" << mod.modifier_index << ")" << std::endl;
      }
      client.Write(move);
      moves = state.Moves();
    } else {
      auto move = client.Read();
      std::cout << "Received move:" << std::endl;
       for(const auto& mod : move) {
        std::cout << "\t" << state.declarations().initial_board().vertices_names().Name(mod.vertex) << " (" << mod.vertex << ") "
           << actions_translator[mod.modifier_index] << " (" << mod.modifier_index << ")" << std::endl;
      }
      state.Apply(move);
      moves = state.Moves();
    }
  }
    std::cout << "\n";
  std::cout << RectangularBoardDescription(state.board_content(), state.declarations()) << std::endl;
  std::cout << "Variables values at end are:" << std::endl;
  std::cout << VariablesValuesDescription(state) << std::endl;
  return 0;
}