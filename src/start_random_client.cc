//
// Created by shrum on 02.07.19.
//

#include <iostream>
#include <memory>
#include <vector>

#include <networking/synchronous_client.h>
#include <rbgParser/src/game_items.hpp>
#include <game_state/construction/game_state_creator.h>
#include <utility/printer.h>

using namespace rbg;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage " << argv[0] << " <host> <port>\n";
    return 1;
  }

  srand(static_cast<unsigned int>(time(nullptr)));

  rbg_parser::messages_container msg;
  SynchronousClient client(argv[argc - 2], argv[argc - 1]);

  GameState state = CreateGameState(client.description());
  auto actions_translator = ActionsDescriptionsMap(client.description());

  player_id_t assigned_player = client.player();

  std::cout << "I am player: " << state.declarations().players_resolver.Name(assigned_player) << " (" << assigned_player << ")"
            << std::endl;

  auto moves = state.Moves();
  while (!moves.empty()) {
    if (state.current_player() == assigned_player) {
      auto move = moves[rand() % moves.size()];
      state.Apply(move);
      std::cout << "Sending move:" << std::endl;
      for(const auto& mod : move) {
        std::cout << "\t" << state.declarations().initial_board.vertices_names().Name(mod.vertex) << " (" << mod.vertex << ") "
           << actions_translator[mod.modifier_index] << " (" << mod.modifier_index << ")" << std::endl;
      }
      client.Write(move);
      moves = state.Moves();
    } else {
      auto move = client.Read();
      state.Apply(move);
      moves = state.Moves();
    }
  }
  std::cout << "Variables values at end are:" << std::endl;
  for(variable_id_t id = 0; id < state.declarations().variables_resolver.size(); id++) {
    auto variable_name = state.declarations().variables_resolver.Name(id);
    std::cout << "\t" << variable_name << " (" << id << ") : " << state.variables_values()[id];
    if(state.declarations().players_resolver.contains(variable_name)) {
      std::cout << " (result for player " << variable_name << " (" << state.declarations().players_resolver.Id(variable_name) << "))";
    }
    std::cout << std::endl;
  }
  return 0;
}