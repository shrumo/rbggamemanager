//
// Created by shrum on 02.07.19.
//

#include <iostream>
#include <memory>
#include <vector>

#include <networking/synchronous_client.h>
#include <rbgParser/src/game_items.hpp>
#include <construction/gamestate_creator.h>

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

  player_id_t assigned_player = client.player();

  std::cout << "I am player: " << state.declarations().players_resolver.Name(assigned_player)
            << std::endl;

  auto moves = state.Moves();
  while (!moves.empty()) {
    if (state.current_player() == assigned_player) {
      auto move = moves[rand() % moves.size()];
      state.Apply(move);
      client.Write(move);
      moves = state.Moves();
    } else {
      auto move = client.Read();
      state.Apply(move);
      moves = state.Moves();
    }
  }

  return 0;
}