#include <cassert>
#include <fstream>
#include <chrono>
#include <random>

#include <game_state/construction/game_state_creator.h>
#include <utility/calculate_perft.h>
#include <utility/printer.h>
#include <utility/shift_table_checker.h>
#include <stl_extension/argparse.h>


namespace rbg {

    // std::vector<node_t> DotsNodes(const Nfa<std::unique_ptr<Move>>& nfa) {
    //     std::vector<node_t> result;
    //     for (node_t node : nfa.graph.nodes()) {
    //         if(nfa.graph.edges_ids_from(node).size() == 1
    //         && nfa.graph.EdgesFrom(node).begin()->content()->type == MoveType::kN
    //         ) 
    //     }
    // }

    std::unordered_map<const rbg_parser::game_move*, bool> MarkRedundantDotsInGame(const std::string& game_text) {
        auto game = ParseGame(game_text);
        Declarations declarations = Declarations(*game);
        rbg::Nfa<std::unique_ptr<rbg::Move>> nfa = CreateNfa(*game->get_moves(), declarations);



        NfaBoardProduct board_product(nfa, declarations.initial_board(), {/* vertex_id= */ 1});
    }
}
