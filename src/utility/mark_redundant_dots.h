#include <game_state/construction/game_state_creator.h>

#include <cassert>
#include <chrono>
#include <fstream>
#include <queue>
#include <random>

namespace rbg {

bool IsModifierNode(const Nfa<std::unique_ptr<Move>> &nfa,
                    const NfaBoardProduct &nfa_board, node_t vertex_node);

bool MultipleModifierPathsForward(const Nfa<std::unique_ptr<Move>> &nfa,
                                  const NfaBoardProduct &board_product,
                                  node_t noop_vertex_node);
bool MultipleModifierPathsBackward(const Nfa<std::unique_ptr<Move>> &nfa,
                                   const NfaBoardProduct &board_product,
                                   node_t noop_vertex_node);

bool IsNoopRedundant(const Nfa<std::unique_ptr<Move>> &nfa,
                     const NfaBoardProduct &board_product, node_t nfa_noop_node,
                     size_t vertices_count);

std::unordered_set<const rbg_parser::game_move *>
RedundantNoopsInGame(const rbg_parser::parsed_game *parsed_game);
} // namespace rbg
