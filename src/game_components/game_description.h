//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_GAME_DESCRIPTION_H
#define RBGGAMEMANAGER_GAME_DESCRIPTION_H


#include "name_resolver.h"
#include "../game_nfa/automaton.h"
#include "../game_nfa/game_moves_description.h"
#include "board.h"
#include "../../rbgParser/src/parsed_game.hpp"

// Declarations keep track of declared token ids.
class Declarations {
public:
  Declarations() = default;

  void AddPlayer(token_id_t player_id) {
    players_ids_.push_back(player_id);
    max_player_id_ = std::max(player_id, max_player_id_);
    min_player_id_ = std::min(player_id, min_player_id_);
  }

  void AddVariable(token_id_t variable_id) {
    variables_ids_.push_back(variable_id);
    max_variable_id_ = std::max(variable_id, max_variable_id_);
    min_variable_id_ = std::min(variable_id, min_variable_id_);
  }

  void AddPiece(token_id_t piece_id) {
    pieces_ids_.push_back(piece_id);
    max_piece_id_ = std::max(piece_id, max_piece_id_);
    min_piece_id_ = std::min(piece_id, min_piece_id_);
  }

  const std::vector<token_id_t> &players_ids() const {
    return players_ids_;
  }

  const std::vector<token_id_t> &variables_ids() const {
    return variables_ids_;
  }

  const std::vector<token_id_t> &pieces_ids() const {
    return pieces_ids_;
  }

  bool IsPlayer(token_id_t token) const {
    return min_player_id_ <= token && token <= max_player_id_;
  }

  bool IsVariable(token_id_t token) const {
    return min_variable_id_ <= token && token <= max_variable_id_;
  }

  bool IsPiece(token_id_t token) const {
    return min_piece_id_ <= token && token <= max_piece_id_;
  }

  token_id_t max_player_id() const {
    return max_player_id_;
  }

  token_id_t max_variable_id() const {
    return max_variable_id_;
  }

  token_id_t max_piece_id() const {
    return max_piece_id_;
  }

private:
  std::vector<token_id_t> players_ids_;
  token_id_t min_player_id_;
  token_id_t max_player_id_;
  std::vector<token_id_t> variables_ids_;
  token_id_t min_variable_id_;
  token_id_t max_variable_id_;
  std::vector<token_id_t> pieces_ids_;
  token_id_t min_piece_id_;
  token_id_t max_piece_id_;
};

class GameDescription {
public:
  token_id_t deterministic_keeper_player_id() const {
    return deterministic_keeper_id_;
  }

  token_id_t nondeterministic_keeper_player_id() const {
    return nondeterministic_keeper_id_;
  }

  const std::string text_description() const {
    return game_text_;
  }

  const Declarations &declarations() const {
    return declarations_;
  }

  const NameResolver &resolver() const {
    return resolver_;
  }

  const GameMovesDescription &moves_description() const {
    return moves_description_;
  }

  const Board &initial_board() const {
    return initial_board_;
  }

  size_t VariablesCount() const {
    return resolver_.NamesCount();
  }

  uint bound() const {
    return bound_;
  }

  friend GameDescription
  CreateDescription(const rbg_parser::parsed_game &game);

private:
  /* TODO(shrum): Make other ways to Create GameDescription */
  GameDescription(NameResolver name_resolver,
                  GameMovesDescription moves_description,
                  Board initial_board, Declarations declarations,
                  std::string game_description,
                  uint bound)
      : resolver_(std::move(name_resolver)),
        moves_description_(std::move(moves_description)),
        initial_board_(std::move(initial_board)),
        declarations_(std::move(declarations)),
        game_text_(game_description),
        deterministic_keeper_id_(resolver_.Id("_epsilon")),
        nondeterministic_keeper_id_(resolver_.Id("_*")),
        bound_(bound) {
  }

  NameResolver resolver_;
  GameMovesDescription moves_description_;
  Board initial_board_;

  Declarations declarations_;

  std::string game_text_;

  token_id_t deterministic_keeper_id_;
  token_id_t nondeterministic_keeper_id_;

  uint bound_;
};


#endif //RBGGAMEMANAGER_GAME_DESCRIPTION_H
