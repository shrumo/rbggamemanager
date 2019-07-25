//
// Created by shrum on 01.07.19.
//



#include <cassert>
#include <fstream>
#include <chrono>

#include <game_state/construction/gamestate_creator.h>
#include <game_state/calculate_perft.h>

using namespace rbg;
using namespace std;

int main(int argc, const char *argv[]) {
  assert((argc == 2 || argc == 3) && "You need to pass the path to the game files and a depth optionally.");

  uint depth = 2;
  if (argc == 3) {
    std::stringstream text(argv[2]);
    text >> depth;
  }

  std::ifstream filestream(argv[1]);
  std::stringstream buffer;
  buffer << filestream.rdbuf();

  auto game = CreateGameState(buffer.str());
  auto begin = std::chrono::system_clock::now();
  auto result = Perft(game, depth);
  auto end = std::chrono::system_clock::now();
  auto duration = std::chrono::duration<double>(end - begin).count();
  std::cout << "Calculated fast perft for depth " << depth << " in "
            << std::fixed << std::showpoint
            << duration
            << "s" << std::endl;
  std::cout << "There are " << result.leaves_count << " leaves" << std::endl;
  std::cout << "Number of traveled states: " << result.nodes_count << " ("
            << std::fixed << std::showpoint
            << result.nodes_count / duration
            << " states/sec)" << std::endl;
}