//
// Created by shrum on 02.07.19.
//

#include <algorithm>
#include "declarations_creator.h"

rbg::Declarations rbg::CreateDeclarations(const rbg_parser::parsed_game &game) {
  NamesResolver pieces_resolver;
  NamesResolver variables_resolver;
  NamesResolver players_resolver;

  std::vector<variable_value_t> variables_bounds(game.get_declarations().get_legal_variables().size() +
                                                 game.get_declarations().get_legal_players().size());
  for (const auto &piece : game.get_declarations().get_legal_pieces()) {
    pieces_resolver.Id(piece.to_string());
  }
  auto players_tokens = std::vector<std::pair<rbg_parser::token, uint>>(
      game.get_declarations().get_legal_players().begin(), game.get_declarations().get_legal_players().end());

  // Add keeper player id, this also makes sure that first player has index 1
  player_id_t keeper_id = players_resolver.Id(">");

  std::sort(players_tokens.begin(), players_tokens.end(),
            [](const std::pair<rbg_parser::token, uint> &a, const std::pair<rbg_parser::token, uint> &b) {
              return a.first.get_position() < b.first.get_position();
            });
  for (const auto &player : players_tokens) {
    auto variable_id = variables_resolver.Id(player.first.to_string());
    players_resolver.Id(player.first.to_string());
    variables_bounds[variable_id] = player.second;
  }
  for (const auto &variable : game.get_declarations().get_legal_variables()) {
    auto variable_id = variables_resolver.Id(variable.first.to_string());
    variables_bounds[variable_id] = variable.second;
  }

  Board board(game.get_board().get_size()+1, game.get_declarations().get_legal_edges().size(), pieces_resolver.size());
  for (const auto &edge : game.get_declarations().get_legal_edges()) {
    board.AddEdgeName(edge.to_string());
  }

  board.AddVertexName("<out>");

  for (uint id = 0; id < game.get_board().get_size(); id++) {
    board.AddVertexName(game.get_board().get_vertex(id).to_string());
  }

  for (uint id = 0; id < game.get_board().get_size(); id++) {
    vertex_id_t source_id = board.vertices_names().Id(game.get_board().get_vertex(id).to_string());
    for (const auto &edge : game.get_board().get_outgoing_edges(id)) {
      vertex_id_t target_id = board.vertices_names().Id(game.get_board().get_vertex(edge.second).to_string());
      board.AddEdge(source_id, board.edges_names().Id(edge.first.to_string()), target_id);
    }
    board.initial_content().set(source_id, pieces_resolver.Id(game.get_board().get_starting_piece(id).to_string()));
  }

  return {pieces_resolver, variables_resolver, players_resolver, variables_bounds, board, keeper_id};
}
