//
// Created by shrum on 30.06.19.
//

#ifndef RBGGAMEMANAGER_CALCULATE_PERFT_H
#define RBGGAMEMANAGER_CALCULATE_PERFT_H

#include "game_state/game_state.h"

namespace rbg {

  struct PerftResult {
    uint leaves_count;
    uint nodes_count;
  };

  PerftResult Perft(GameState &state, size_t depth) {
    if (depth == 0) {
      return {1, 1};
    }
    auto moves = state.Moves();
    PerftResult result = {0, 0};
    result.nodes_count += 1;
    for (const auto &move : std::vector<GameMove>(moves.begin(), moves.end())) {
      auto revert_info = state.Apply(move);
      auto rec_result = Perft(state, depth - 1);
      result.leaves_count += rec_result.leaves_count;
      result.nodes_count += rec_result.nodes_count;
      state.Revert(revert_info);
    }
    return result;
  }
}

#endif //RBGGAMEMANAGER_CALCULATE_PERFT_H
