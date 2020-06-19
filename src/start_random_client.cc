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
    std::cout << "Usage: " << argv[0] << " <host> <port> [--seed <random_seed>] [--sleep <sleep_time ms>] [--time <time_to_disconnect ms>] [--additional_preparation_time <preparation_time ms>]" << std::endl;
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
    sleep_time = std::stod(args.flags.at("sleep"))/1000.0; // Internally store times in seconds
  }

  double time_to_disconnect = 0.0;
  if(args.flags.find("time") != args.flags.end()) {
    time_to_disconnect = std::stod(args.flags.at("time"))/1000.0; // Internally store times in seconds
  }

  double additional_compilation_time = 0.0;
  if(args.flags.find("additional_preparation_time") != args.flags.end()) {
    additional_compilation_time = std::stod(args.flags.at("additional_preparation_time"))/1000.0;
  }

  rbg_parser::messages_container msg;
  Client client(args.positional_args[0], args.positional_args[1]);

  GameState state = CreateGameState(client.description());
  auto actions_translator = ActionsDescriptionsMap(client.description());

  player_id_t assigned_player = client.player();

  std::cout << "I am player: " << state.declarations().players_resolver().Name(assigned_player) << " (" << assigned_player << ")\n"; 
  std::cout << "I had " << client.preparation_time() << " seconds to prepare to play" << std::endl;

  std::this_thread::sleep_for(std::chrono::duration<double>(additional_compilation_time));
  
  client.Ready();
 
  auto first_game_begin = std::chrono::system_clock::now();
  do {
  auto moves = state.Moves();
  while (!moves.empty()) {
    if (state.current_player() == assigned_player) {
      std::cout << "Its my turn, waiting for deadline from server." << std::endl;
      double deadline = client.ReadDeadline();
      std::cout << "The deadline to make the move is " << deadline << " seconds." << std::endl;
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
      // This sleep is to pretend I am doing something important
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
  state.Reset();
  client.ReadReset();
  } while(std::chrono::duration<double>(std::chrono::system_clock::now() - first_game_begin).count() < time_to_disconnect);
  return 0;
}
