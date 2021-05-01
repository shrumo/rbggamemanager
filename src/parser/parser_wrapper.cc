//
// Created by shrum on 04.06.19.
//

#include "parser_wrapper.h"

std::unique_ptr<rbg_parser::parsed_game>
rbg::ParseGame(const std::string &game_text) {
  rbg_parser::messages_container msg;

  std::unique_ptr<rbg_parser::parsed_game> pg;

  std::vector<rbg_parser::token> result = tokenize(game_text, msg);
  rbg_parser::game_items g = input_tokens(result, msg);
  pg = std::make_unique<rbg_parser::parsed_game>(g.parse_game(msg));

  return pg;
}
