//
// Created by shrum on 22/07/19.
//

#include <cassert>
#include <fstream>
#include <chrono>
#include <random>

#include <game_state/construction/game_state_creator.h>
#include <utility/calculate_perft.h>
#include <utility/printer.h>
#include <stl_extension/argparse.h>


using namespace rbg;
using namespace std;

int main(int argc, const char *argv[]) {
  auto args = std_ext::parse_args(argc, argv);
  
  if (args.positional_args.size() != 1) {
    std::cout << "Usage: " << argv[0] << " <game_file> [--seed <random_seed>]" << std::endl;
    return 0;
  }

  uint seed = 0;
  if (args.flags.find("seed") != args.flags.end()) {
    seed = std::stoi(args.flags["seed"]);
  } 

  mt19937 rng(seed);

  std::ifstream file_stream(args.positional_args[0]);
  std::stringstream buffer;
  buffer << file_stream.rdbuf();

  auto game = CreateGameState(buffer.str());
  auto actions_translator = ActionsDescriptionsMap(buffer.str());
  auto moves = game.Moves();

  std::cout << game.declarations().initial_board().vertices_names() << std::endl;

  while(!moves.empty()) {
    uniform_int_distribution<> random_index(0, moves.size()-1);
    cout << "Current player is: " << game.declarations().players_resolver().Name(game.current_player()) << " (" << game.current_player() << ")" << endl;

    cout << RectangularBoardDescription(game.board_content(), game.declarations());
    cout << "Variables values are:" << std::endl;
    cout << VariablesValuesDescription(game) << std::endl;
    auto move = moves[random_index(rng)];
    cout << "Chosen move:" << std::endl;
    for(const auto& mod : move) {
      cout << "\t" << game.declarations().initial_board().vertices_names().Name(mod.vertex) << " (" << mod.vertex << ") "
           << actions_translator[mod.modifier_index] << " (" << mod.modifier_index << ")" << std::endl;
    }
    game.Apply(move);
    moves = game.Moves();
    cout << endl;
  }


}
