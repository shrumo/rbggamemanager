//
// Created by shrum on 01.07.19.
//

#include <game_state/construction/game_state_creator.h>
#include <stl_extension/argparse.h>
#include <utility/calculate_perft.h>
#include <utility/calculate_sim.h>

#include <cassert>
#include <chrono>
#include <fstream>

using namespace rbg;
using namespace std;

int main(int argc, const char *argv[]) {
  auto args = std_ext::parse_args(argc, argv);

  if (args.positional_args.size() != 1) {
    std::cout << "Usage: " << argv[0] << " <game_file> [--depth <search_depth>] [--sim <sim_count>] [--seed <seed_uint>]"
              << std::endl;
    return 0;
  }

  uint depth = 0;
  if (args.flags.find("depth") != args.flags.end()) {
    depth = std::stoi(args.flags["depth"]);
  }
  uint sim_count = 0;
  if (args.flags.find("sim") != args.flags.end()) {
    sim_count = std::stoi(args.flags["sim"]);
  }
  if (depth == 0 && sim_count == 0) {
    std::cout << "Either depth or sim must be used" << std::endl;
    return 1;
  } else
  if (depth > 0 && sim_count > 0) {
    std::cout << "Depth and sim cannot be used together" << std::endl;
    return 1;
  }
  
  std::ifstream filestream(args.positional_args[0]);
  std::stringstream buffer;
  buffer << filestream.rdbuf();

  auto begin = std::chrono::system_clock::now();
  auto game = CreateGameState(buffer.str());
  auto end = std::chrono::system_clock::now();
  auto duration = std::chrono::duration<double>(end - begin).count();
  std::cout << "Created the game state in " << std::fixed << std::showpoint << duration << "s" << std::endl;

  if (depth > 0) {
    begin = std::chrono::system_clock::now();
    auto result = Perft(game, depth);
    end = std::chrono::system_clock::now();
    duration = std::chrono::duration<double>(end - begin).count();
    std::cout << "Calculated perft for depth " << depth << " in " << std::fixed
              << std::showpoint << duration << "s" << std::endl;
    std::cout << "There are " << result.leaves_count << " leaves" << std::endl;
    std::cout << "Number of traveled states: " << result.nodes_count << " ("
              << std::fixed << std::showpoint << result.nodes_count / duration
              << " states/sec)" << std::endl;
  } else {
    uint seed;
    if (args.flags.find("seed") != args.flags.end()) {
      seed = std::stoi(args.flags["seed"]);
    } else {
      seed = std::random_device()();
    }

    begin = std::chrono::system_clock::now();
    SimResult result = RandomSimulations(game, sim_count, seed);
    end = std::chrono::system_clock::now();
    duration = std::chrono::duration<double>(end - begin).count();
    std::cout << "Calculated " << sim_count << " random simulations in " << std::fixed
              << std::showpoint << duration << "s" << std::endl;
    std::cout << "Number of traveled states: " << result.nodes_count << " ("
              << std::fixed << std::showpoint << result.nodes_count / duration
              << " states/sec)" << std::endl;
    std::cout << "Average depth: " << static_cast<double>(result.nodes_count) / sim_count << std::endl;
    std::cout << "Average goals:";
    for (uint r = 0; r < result.sum_goals.size(); r++) {
      std::cout << " " << static_cast<double>(result.sum_goals[r]) / sim_count;
    }
    std::cout << std::endl;
  }
}
