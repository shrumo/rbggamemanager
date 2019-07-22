//
// Created by shrum on 22/07/19.
//

#include <cassert>
#include <fstream>
#include <chrono>
#include <random>

#include <construction/gamestate_creator.h>
#include <game_state/calculate_perft.h>
#include <construction/moves/moves_printer.h>


using namespace rbg;
using namespace std;

std::unordered_map<uint,string> ActionsDescriptionsMap(const string& game_text) {
  std::unordered_map<uint, string> result;
  auto game = ParseGame(ParseGame(game_text)->to_rbg());
  Declarations declarations = CreateDeclarations(*game);
  VisitedChecksNfa nfa = CreateVisitedChecksNfa(*game->get_moves(), declarations);
  for(node_t node: nfa.nfa.graph.nodes()) {
    for(const auto& edge : nfa.nfa.graph.EdgesFrom(node)) {
      if(edge.content()->indexed()) {
        const auto& indexed_edge = dynamic_cast<const IndexedMove&>(*edge.content());
        result[indexed_edge.index()] = MoveDescription(*edge.content(), declarations);
      }
    }
  }
  return result;
}

int main(int argc, const char *argv[]) {
  assert((argc == 2 || argc == 3) && "You need to pass the path to the game file and seed for random optionally.");

  uint seed = 2;
  if (argc == 3) {
    std::stringstream text(argv[2]);
    text >> seed;
  }

  mt19937 rng(seed);

  std::ifstream filestream(argv[1]);
  std::stringstream buffer;
  buffer << filestream.rdbuf();

  auto game = CreateGameState(buffer.str());
  auto actions_translator = ActionsDescriptionsMap(buffer.str());
  auto moves = game.Moves();
  while(!moves.empty()) {
    uniform_int_distribution<> random_index(0, moves.size()-1);
    cout << "Current player is: " << game.declarations().players_resolver.Name(game.current_player()) << " (" << game.current_player() << ")" << endl;
    cout << "Variables values are:" << std::endl;
    for(variable_id_t id = 0; id < game.declarations().variables_resolver.size(); id++) {
      cout << "\t" << game.declarations().variables_resolver.Name(id) << " (" << id << ") : " << game.variables_values()[id] << std::endl;
    }
    cout << RectangularBoardDescription(game.board_content(), game.declarations());
    auto move = moves[random_index(rng)];
    cout << "Chosen move:" << std::endl;
    for(const auto& mod : move) {
      cout << "\t" << game.declarations().board_description.vertices_names().Name(mod.vertex) << " (" << mod.vertex << ") "
           << actions_translator[mod.modifier_index] << " (" << mod.modifier_index << ")" << std::endl;
    }
    game.Apply(moves[random_index(rng)]);
    moves = game.Moves();
    cout << endl;
  }


}
