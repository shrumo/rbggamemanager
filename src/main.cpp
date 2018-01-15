//
// Created by shrum on 12.01.18.
//

#include <array>
#include <iostream>

#include "parser_connection/parser_connection.h"

#include<fstream>
#include<sstream>
#include <chrono>

#include"../rbgParser/src/game_items.hpp"

#include "boost/program_options.hpp"

#include "game_components/game_description.h"
#include "game_components/game_state.h"

int main(int argc, char *argv[]) {
    rbg_parser::messages_container msg;
    std::string input_file_name(argv[argc - 1]);
    std::ifstream t(input_file_name);
    std::stringstream buffer;
    buffer << t.rdbuf();

    std::vector<rbg_parser::token> result = tokenize(buffer.str(), msg);
    rbg_parser::game_items g = input_tokens(result, msg);
    rbg_parser::parsed_game pg = g.parse_game(msg);

    game_description gd(create_description(pg));
    game_state state(gd);
    auto moves = state.get_move_evaluator().find_moves(&state);
    std::cout << "DONE" << std::endl;
}