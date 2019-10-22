//
// Created by shrum on 02.07.19.
//

#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <string>

#include <networking/client.h>
#include <rbgParser/src/game_items.hpp>
#include <game_state/construction/game_state_creator.h>
#include <utility/printer.h>
#include <stl_extension/argparse.h>

using namespace rbg;

int main(int argc, const char *argv[]) {
  auto args = std_ext::parse_args(argc, argv);
  
  if (args.positional_args.size() != 2) {
    std::cout << "Usage: " << argv[0] << " <host> <port> [--seed <random_seed>] [--sleep <sleep_time>]" << std::endl;
    return 0;
  }

  if(args.flags.find("seed") != args.flags.end()) {
    srand(std::stoi(args.flags.at("seed")));
  }
  else {
    srand(static_cast<unsigned int>(time(nullptr)));
  }

  double sleep_time = 0.0;
  if(args.flags.find("sleep") != args.flags.end()) {
    sleep_time = std::stod(args.flags.at("sleep"));
  }

  rbg_parser::messages_container msg;
  Client client(args.positional_args[0], args.positional_args[1]);

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
      std::this_thread::sleep_for(std::chrono::duration<double>(sleep_time));
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
