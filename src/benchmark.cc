//
// Created by shrum on 01.07.19.
//

#include <cassert>
#include <fstream>
#include <chrono>

#include <game_state/construction/game_state_creator.h>
#include <utility/calculate_perft.h>
#include <stl_extension/argparse.h>

using namespace rbg;
using namespace std;

int main(int argc, const char *argv[]) { 
  auto args = std_ext::parse_args(argc, argv);
  
  if (args.positional_args.size() != 1) {
    std::cout << "Usage: " << argv[0] << " <game_file> [--depth <search_depth>]" << std::endl;
    return 0;
  }

  uint depth = 2;
  if (args.flags.find("depth") != args.flags.end()) {
    depth = std::stoi(args.flags["depth"]);
  }

  std::ifstream filestream(args.positional_args[0]);
  std::stringstream buffer;
  buffer << filestream.rdbuf();

  auto begin = std::chrono::system_clock::now();
  auto game = CreateGameState(buffer.str());
  auto end = std::chrono::system_clock::now();
  auto duration = std::chrono::duration<double>(end - begin).count();
   std::cout << "Created the game state in in "
            << std::fixed << std::showpoint
            << duration
            << "s" << std::endl;
  begin = std::chrono::system_clock::now();
  auto result = Perft(game, depth);
  end = std::chrono::system_clock::now();
  duration = std::chrono::duration<double>(end - begin).count();
  std::cout << "Calculated perft for depth " << depth << " in "
            << std::fixed << std::showpoint
            << duration
            << "s" << std::endl;
  std::cout << "There are " << result.leaves_count << " leaves" << std::endl;
  std::cout << "Number of traveled states: " << result.nodes_count << " ("
            << std::fixed << std::showpoint
            << result.nodes_count / duration
            << " states/sec)" << std::endl;
}
