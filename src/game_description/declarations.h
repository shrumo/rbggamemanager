//
// Created by shrum on 11.06.19.
//

#ifndef RBGGAMEMANAGER_DECLARATIONS_H
#define RBGGAMEMANAGER_DECLARATIONS_H

#include <rbgParser/src/parsed_game.hpp>
#include <algorithm>
#include "names_resolver.h"
#include "board.h"

namespace rbg {
  using variable_id_t = name_id_t;
  using player_id_t = name_id_t;
  using variable_value_t = uint;

  class Declarations {
  public:
    explicit Declarations(const rbg_parser::parsed_game &game)
        : variables_bounds_(game.get_declarations().get_legal_variables().size() +
                           game.get_declarations().get_legal_players().size()),
          initial_board_(game.get_board().get_size() + 1,
                        game.get_declarations().get_legal_edges().size(),
                        game.get_declarations().get_legal_pieces().size()),
          keeper_id_{0} {
      for (const auto &piece : game.get_declarations().get_legal_pieces()) {
        pieces_resolver_.Id(piece.to_string());
      }
      auto players_tokens = std::vector<std::pair<rbg_parser::token, uint>>(
          game.get_declarations().get_legal_players().begin(), game.get_declarations().get_legal_players().end());

      // Add keeper player id, this also makes sure that first player has index 1.
      keeper_id_ = players_resolver_.Id(">");

      std::sort(players_tokens.begin(), players_tokens.end(),
                [](const std::pair<rbg_parser::token, uint> &a, const std::pair<rbg_parser::token, uint> &b) {
                  return a.first.get_position() < b.first.get_position();
                });
      for (const auto &player : players_tokens) {
        auto variable_id = variables_resolver_.Id(player.first.to_string());
        players_resolver_.Id(player.first.to_string());
        variables_bounds_[variable_id] = player.second;
      }
      for (const auto &variable : game.get_declarations().get_legal_variables()) {
        auto variable_id = variables_resolver_.Id(variable.first.to_string());
        variables_bounds_[variable_id] = variable.second;
      }

      for (const auto &edge : game.get_declarations().get_legal_edges()) {
        initial_board_.AddEdgeName(edge.to_string());
      }

      // This makes sure the vertices are indexed starting from 1.
      initial_board_.AddVertexName("<out>");

      for (uint id = 0; id < game.get_board().get_size(); id++) {
        initial_board_.AddVertexName(game.get_board().get_vertex(id).to_string());
      }

      for (uint id = 0; id < game.get_board().get_size(); id++) {
        vertex_id_t source_id = initial_board_.vertices_names().Id(game.get_board().get_vertex(id).to_string());
        for (const auto &edge : game.get_board().get_outgoing_edges(id)) {
          vertex_id_t target_id = initial_board_.vertices_names().Id(
              game.get_board().get_vertex(edge.second).to_string());
          initial_board_.AddEdge(source_id, initial_board_.edges_names().Id(edge.first.to_string()), target_id);
        }
        initial_board_.initial_content().set(source_id,
                                            pieces_resolver_.Id(game.get_board().get_starting_piece(id).to_string()));
      }
    }

    const NamesResolver pieces_resolver() const {
      return pieces_resolver_;
    }

    const NamesResolver variables_resolver() const {
      return variables_resolver_;
    }

    const NamesResolver players_resolver() const {
      return players_resolver_;
    }

    variable_value_t variable_bound(variable_id_t variable) const {
      return variables_bounds_[variable];
    }

    const Board& initial_board() const {
      return initial_board_;
    }

    player_id_t keeper_id() const {
      return keeper_id_;
    }

  private:
    NamesResolver pieces_resolver_;
    NamesResolver variables_resolver_;
    NamesResolver players_resolver_;
    std::vector<variable_value_t> variables_bounds_;
    Board initial_board_;
    player_id_t keeper_id_;
  };
}

#endif //RBGGAMEMANAGER_DECLARATIONS_H
