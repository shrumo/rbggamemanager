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
#include <networking/socket.h>


using namespace rbg;
using namespace std;

int main(int argc, const char *argv[]) {
  auto args = std_ext::parse_args(argc, argv);

  if (args.positional_args.size() != 1) {
    std::cout << "Usage: " << argv[0] << " <game_file>" << std::endl;
    return 0;
  }

  std::ifstream file_stream(args.positional_args[0]);
  std::stringstream buffer;
  buffer << file_stream.rdbuf();

  auto game = CreateGameState(buffer.str());
  auto actions_translator = ActionsDescriptionsMap(buffer.str());
  while(true) {
    std::cout << "Put in move encoding: " << std::endl;
    std::string move_encoded;
    std::getline(std::cin, move_encoded);
    auto move = DecodeMove(move_encoded);
    for (const auto &mod : move) {
      cout << "\t" << game.declarations().initial_board().vertices_names().Name(mod.vertex) << " (" << mod.vertex
           << ") "
           << actions_translator[mod.modifier_index] << " (" << mod.modifier_index << ")" << std::endl;
    }
  }
}
