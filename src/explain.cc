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

bool MoveExists(const std::vector<GameMove> &moves, const GameMove &checked_move) {
  std::vector<GameMove> result;
  for (const auto& move : moves) {
    if(move == checked_move) return true;
  }
  return false;
}


std::vector<GameMove> MovesWithPrefix(const std::vector<GameMove> &moves, const GameMove &move_prefix) {
  std::vector<GameMove> result;
  for (const auto& move : moves) {
    if(move.size() < move_prefix.size()) {
      continue;
    }
    bool equal = true;
    for(size_t i = 0; i < move_prefix.size(); i++) {
      if( ! (move[i] == move_prefix[i]) ) {
        equal = false;
        break;
      }
    }
    if(equal) {
      result.push_back(move);
    }
  }
  return result;
}

std::unordered_set<ModifierApplication> PossibleNextModifiers(const std::vector<GameMove> &moves, const GameMove &move_prefix) {
  std::unordered_set<ModifierApplication> result;
  for (const auto& move : moves) {
    if(move.size() <= move_prefix.size()) {
      continue;
    }
    bool equal = true;
    for(size_t i = 0; i < move_prefix.size(); i++) {
      if(!(move[i] == move_prefix[i])) {
        equal = false;
        break;
      }
    }
    if(equal) {
      if(move.size() > move_prefix.size()) {
        result.insert(move[move_prefix.size()]);
      }
    }
  }
  return result;
}

int main(int argc, const char *argv[]) {
  auto args = std_ext::parse_args(argc, argv);
  
  if (args.positional_args.size() != 1) {
    std::cout << "Usage: " << argv[0] << " <game_file> [--seed <random_seed>] [--interactive (no|help|full)]" << std::endl;
    return 0;
  }

  uint seed = 0;
  if (args.flags.find("seed") != args.flags.end()) {
    seed = std::stoi(args.flags["seed"]);
  }

  std::string interactive = "no";
  if (args.flags.find("interactive") != args.flags.end()) {
    interactive = args.flags.at("interactive");
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
    auto move = moves[0];
    if(interactive == "help") {
      std::cout << "Which move do you want to choose: " << std::endl;
      std::cout << -1 << ") random" << std::endl;
      for(size_t i = 0; i < moves.size(); i++) {
        std::cout << i << ") ";
        for(const auto& mod : moves[i]) {
          cout << "\t" << game.declarations().initial_board().vertices_names().Name(mod.vertex) << " (" << mod.vertex
               << ") "
               << actions_translator[mod.modifier_index] << " (" << mod.modifier_index << ")" << std::endl;
        }
      }
      int index;
      cin >> index;
      move = moves[index];
      if (index == -1) {
        move = moves[random_index(rng)];
      }
    }
    else if (interactive == "full") {
      std::cout << "Put in a move encoding: (you can do it partially) " << std::endl;
      move = GameMove();
      std::string append_move_encoded;
      while(!MoveExists(moves,move) || !append_move_encoded.empty()) {
        if(MovesWithPrefix(moves, move).size() == 0) {
          std::cout << "Resetting." << std::endl;
          move = GameMove();
        }

        std::cout << "You put in so far: " << std::endl;
        for (const auto &mod : move) {
          cout << "\t" << game.declarations().initial_board().vertices_names().Name(mod.vertex) << " (" << mod.vertex
               << ") "
               << actions_translator[mod.modifier_index] << " (" << mod.modifier_index << ")" << std::endl;
        }

        std::cout << "Next possible modifiers: " << std::endl;
        for (const auto &mod : PossibleNextModifiers(moves, move)) {
          cout << "\t" << game.declarations().initial_board().vertices_names().Name(mod.vertex) << " (" << mod.vertex
               << ") "
               << actions_translator[mod.modifier_index] << " (" << mod.modifier_index << ")" << std::endl;
        }

        std::cout << "Put in next vertex modifier or leave empty: " << std::endl;
        std::getline(std::cin, append_move_encoded);
        auto move_append = DecodeMove(append_move_encoded);
        move.insert(move.end(),move_append.begin(), move_append.end());
      }
    }
    else {
      move = moves[random_index(rng)];
    }
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
