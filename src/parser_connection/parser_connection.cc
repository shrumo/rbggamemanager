//
// Created by shrum on 13.01.18.
//

#include "parser_connection.h"
#include "../game_components/board.h"
#include "../game_components/game_description.h"

Board CreateBoard(const rbg_parser::game_board &parsed_board,
                  const NameResolver &resolver) {
  Board result(parsed_board.get_width(), parsed_board.get_height());
  for (size_t y = 0; y < result.height(); y++) {
    for (size_t x = 0; x < result.width(); x++) {
      std::string name = parsed_board.get_square((uint) x,
                                                 (uint) y).to_string();
      result(x, y) = resolver.Id(name);
    }
  }
  return result;
}

NameResolver CreateResolver(const rbg_parser::declarations &declarations) {
  NameResolver result;
  for (const auto &piece_token : declarations.get_legal_pieces()) {
    std::string piece_name = piece_token.to_string();
    result.AddName(piece_name);
  }
  for (const auto &player_token : declarations.get_legal_players()) {
    std::string player_name = player_token.to_string();
    result.AddName(player_name);
  }
  for (const auto &variable_token : declarations.get_legal_variables()) {
    std::string variable_name = variable_token.to_string();
    result.AddName(variable_name);
  }
  result.AddName("_epsilon");
  result.AddName("_*");
  return result;
}


GameMovesDescription
create_moves(const rbg_parser::game_move &move, const NameResolver &resolver,
             token_id_t piece_id_threshold) {
  GameNfaCreator creator(resolver, piece_id_threshold);
  move.accept(creator);
  return creator.ExtractDescription();
}


GameDescription CreateDescription(const rbg_parser::parsed_game &game) {
  NameResolver resolver(CreateResolver(game.get_declarations()));
  Board initial_board(CreateBoard(game.get_board(), resolver));
  GameMovesDescription moves(create_moves(*game.get_moves(), resolver,
                                          game.get_declarations().get_legal_pieces().size()));
  Declarations declarations(
      CreateDeclarations(game.get_declarations(), resolver));
  return std::move(GameDescription(std::move(resolver), std::move(moves),
                                   std::move(initial_board),
                                   std::move(declarations), game.to_rbg(true),
                                   game.get_bound()));
}

Declarations CreateDeclarations(const rbg_parser::declarations &declarations,
                                const NameResolver &resolver) {
  Declarations result;
  for (const auto &piece_token : declarations.get_legal_pieces()) {
    std::string piece_name = piece_token.to_string();
    result.AddPiece(resolver.Id(piece_name));
  }
  for (const auto &player_token : declarations.get_legal_players()) {
    std::string player_name = player_token.to_string();
    result.AddPlayer(resolver.Id(player_name));
  }
  for (const auto &variable_token : declarations.get_legal_variables()) {
    std::string variable_name = variable_token.to_string();
    result.AddVariable(resolver.Id(variable_name));
  }
  return result;
}
