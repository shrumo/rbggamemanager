//
// Created by shrum on 12.01.18.
//

#include <array>
#include <iostream>

#include <fstream>
#include <sstream>
#include <chrono>

#include <unordered_map>
#include <memory>
#include <limits>
#include <string>
#include <vector>

#include"../rbgParser/src/game_items.hpp"

#include "parser_connection/parser_connection.h"

#include "boost/program_options.hpp"

#include "game_components/game_description.h"
#include "game_components/game_state.h"

namespace po = boost::program_options;
using uint = unsigned int;

struct PlayerResults {
  int sum, min, max;
};

struct PerftResult {
  size_t leaf_count;
  size_t node_count;
};

PerftResult perft(SearchContext *context, GameState *state, size_t depth) {
  if (depth == 0) {
    return {1, 1};
  }

  std::vector<Move> moves;
  size_t new_depth = depth - 1;
  if (state->player() ==
      state->description().deterministic_keeper_player_id())
  {
    moves = state->FindFirstMove(context);
    new_depth = depth;
  } else if (state->player() == state->description().nondeterministic_keeper_player_id())
  {
    moves = state->FindMoves(context);
    new_depth = depth;
  } else
  {
    moves = state->FindMoves(context);
  }
  size_t leaf_count = 0;
  size_t node_count = 1;
  for (const auto &move : moves) {
    auto revert_info = state->MakeRevertibleMove(move);
    auto rec_res = perft(context, state, new_depth);
    leaf_count += rec_res.leaf_count;
    node_count += rec_res.node_count;
    state->RevertMove(revert_info);
  }
  return {leaf_count, node_count};
}

void
random_play_benchmark(const rbg_parser::parsed_game &pg, size_t iterations) {
  GameDescription gd = CreateDescription(pg);

  std::unordered_map<token_id_t, PlayerResults> player_scores_sum;

  for (const auto &token : pg.get_declarations().get_legal_players()) {
    std::string name = token.first.to_string();
    token_id_t player_id = gd.resolver().Id(name);
    player_scores_sum[player_id] = {0, std::numeric_limits<int>::max(),
                                    std::numeric_limits<int>::min()};
  }

  size_t turns = 0;
  size_t avgmoves = 0;
  size_t all_moves_count = 0;
  auto begin = std::chrono::system_clock::now();
  SearchContext context;
  for (size_t i = 0; i < iterations; i++) {
    size_t moves_count = 0;
    size_t state_turns = 0;
    GameState state(gd);
    auto moves = state.FindMoves(&context);
    moves_count += moves.size();
    if (iterations == 1)
      std::cout << state << std::endl;
    while (!moves.empty()) {
      if (iterations == 1)
        std::cout << "\n";
      state.MakeMove(moves[rand() % moves.size()]);
      if (iterations == 1) {
        std::cout << state << std::endl;
      }
      if (state.player() ==
          state.description().deterministic_keeper_player_id()) {
        moves = state.FindFirstMove(&context);
      } else {
        moves = state.FindMoves(&context);
      }
      moves_count += moves.size();
      state_turns++;
    }
    for (auto &player_score : player_scores_sum) {
      const auto &score = state.Value(player_score.first);
      player_score.second.sum += score;
      if (score < player_score.second.min)
        player_score.second.min = score;
      if (score > player_score.second.max)
        player_score.second.max = score;
    }
    turns += state_turns + 1;
    avgmoves += moves_count / (state_turns + 1);
    all_moves_count += moves_count;
  }
  auto end = std::chrono::system_clock::now();
  auto duration = std::chrono::duration<double>(end - begin).count();
  std::cout << "Calculated " << iterations << " games in " << duration << "s"
            << std::endl;
  std::cout << "Time for one game: " << duration / iterations << "s" << " ("
            << iterations / duration << " games/sec)"
            << std::endl;
  std::cout << "Avarage number of turns in game: "
            << static_cast<double>(turns) / iterations << std::endl;
  std::cout << "Avarage number of moves in one state_: "
            << static_cast<double>(avgmoves) / iterations << std::endl;
  std::cout << "Number of traveled states: " << turns << " ("
            << turns / duration
            << " states/sec)" << std::endl;
  std::cout << "Number of calculated moves: " << all_moves_count << " ("
            << all_moves_count / duration
            << " moves/sec)" << std::endl;
  std::cout << "Avarage player scores: " << "\n";
  for (auto &player_score : player_scores_sum) {
    std::cout << "\t" << gd.resolver().Name(player_score.first) << " : "
              << static_cast<double>(player_score.second.sum) / iterations
              << " (Min: " << player_score.second.min << ", Max: "
              << player_score.second.max << ")\n";
  }
}

void perft_benchmark(const rbg_parser::parsed_game &pg, size_t depth) {
  GameDescription gd = CreateDescription(pg);
  auto begin = std::chrono::system_clock::now();
  SearchContext context;
  GameState state(gd);
  auto result = perft(&context, &state, depth);
  auto end = std::chrono::system_clock::now();
  auto duration = std::chrono::duration<double>(end - begin).count();
  std::cout << "Calculated perft for depth " << depth << " in " << duration
            << "s" << std::endl;
  std::cout << "There are " << result.leaf_count << " leaves" << std::endl;
  std::cout << "Number of traveled states: " << result.node_count << " ("
            << result.node_count / duration
            << " states/sec)" << std::endl;
}

int main(int argc, const char *argv[]) {
  po::options_description description("Allowed options");
  description.add_options()
      ("number,n", po::value<uint>(), "number of games to be played")
      ("help,h", "produce help message")
      ("input-file,i", po::value<std::string>(), "input file")
      ("randomseed,s", po::value<uint>(), "random seed for random player")
      ("depth,d", po::value<uint>(), "depth of perft calculation");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, description), vm);
  po::notify(vm);

  po::positional_options_description p;
  p.add("input-file", -1);

  po::store(po::command_line_parser(argc, argv).
      options(description).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("randomseed")) {
    srand(vm["randomseed"].as<uint>());
  }
  if (vm.count("help") || !vm.count("input-file")) {
    std::cout << description << std::endl;
    return 1;
  }

  rbg_parser::messages_container msg;
  std::ifstream t(vm["input-file"].as<std::string>());
  std::stringstream buffer;
  buffer << t.rdbuf();

  std::unique_ptr<rbg_parser::parsed_game> pg;

  try {
    std::vector<rbg_parser::token> result = tokenize(buffer.str(), msg);
    rbg_parser::game_items g = input_tokens(result, msg);
    pg = std::unique_ptr<rbg_parser::parsed_game>(
        new rbg_parser::parsed_game(g.parse_game(msg)));
  } catch (rbg_parser::message &m) {
    std::cout << "Game description is bad. Here is the error: " << "\n";
    std::cout << m.as_error() << std::endl;
    return 1;
  }

  if (vm.count("depth")) {
    size_t depth = vm["depth"].as<uint>();
    perft_benchmark(*pg, depth);
  } else {
    size_t iterations = 1;
    if (vm.count("number"))
      iterations = vm["number"].as<uint>();
    random_play_benchmark(*pg, iterations);
  }
}
