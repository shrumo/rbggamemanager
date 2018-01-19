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

namespace po = boost::program_options;
using uint = unsigned int;

int main(int argc,const char *argv[]) {
    po::options_description description("Allowed options");
    description.add_options()
            ("number,n",po::value<uint>(),"number of games to be played")
            ("help,h","produce help message")
            ("input-file,i",po::value<std::string>(),"input file")
            ("randomseed,s",po::value<uint>(),"random seed for random player")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc,argv,description), vm);
    po::notify(vm);

    po::positional_options_description p;
    p.add("input-file", -1);

    po::store(po::command_line_parser(argc, argv).
            options(description).positional(p).run(), vm);
    po::notify(vm);

    if(vm.count("randomseed")) {
        srand(vm["randomseed"].as<uint>());
    }
    if(vm.count("help")) {
        std::cout << description << std::endl;
        return 1;
    }

    rbg_parser::messages_container msg;
    std::ifstream t(vm["input-file"].as< std::string>());
    std::stringstream buffer;
    buffer << t.rdbuf();

    std::unique_ptr<rbg_parser::parsed_game> pg;

    try {
        std::vector<rbg_parser::token> result = tokenize(buffer.str(), msg);
        rbg_parser::game_items g = input_tokens(result, msg);
        pg = std::unique_ptr<rbg_parser::parsed_game>(new rbg_parser::parsed_game(g.parse_game(msg)));
    }catch(rbg_parser::message& m){
        std::cout<<"Game description is bad. Here is the error: " << "\n";
        std::cout<<m.as_error()<<std::endl;
        return 1;
    }

    game_description gd(create_description(*pg));

    std::unordered_map<token_id_t, size_t> player_scores;

    for(const auto& token : pg->get_declarations().get_legal_players())
    {
        std::string name = token.to_string();
        token_id_t player_id = gd.get_resolver().id(name);
        player_scores[player_id] = 0;
    }

    size_t turns = 0;
    size_t iterations = 1;
    if(vm.count("number"))
        iterations = vm["number"].as<uint>();
    size_t avgmoves = 0;
    size_t all_moves_count = 0;
    auto begin = std::chrono::system_clock::now();
    for(size_t i = 0; i < iterations; i++) {
        size_t moves_count = 0;
        game_state state(gd);
        auto moves = state.get_move_evaluator().find_moves(&state);
        moves_count += moves.size();
        if(iterations == 1)
            std::cout << state << std::endl;
        while (!moves.empty()) {
            if(iterations == 1)
                std::cout << "\n";
            state.make_move(moves[rand() % moves.size()]);
            if(iterations == 1)
                std::cout << state << std::endl;
            moves = state.get_move_evaluator().find_moves(&state);
            moves_count += moves.size();
        }
        for(auto& player_score : player_scores)
        {
            player_score.second += state.value(player_score.first);
        }
        turns += state.turn() - 1;
        avgmoves += moves_count / (state.turn() - 1);
        all_moves_count += moves_count;
    }
    auto end = std::chrono::system_clock::now();
    std::cout << "Calculated " << iterations << " games in " << std::chrono::duration<double>(end - begin).count() << "s" << std::endl;
    std::cout << "Time for one game: " << std::chrono::duration<double>(end - begin).count() / iterations << "s" << std::endl;
    std::cout << "Avarage number of turns in game: " << (double) turns / iterations << std::endl;
    std::cout << "Avarage number of moves in one state: " << (double) avgmoves / iterations << std::endl;
    std::cout << "Number of calculated moves: " << all_moves_count << std::endl;
    std::cout << "Avarage player scores: " << "\n";
    for(auto& player_score : player_scores)
    {
        std::cout << "\t" << gd.get_resolver().name(player_score.first) << " : " << (double) player_score.second / iterations<< "\n";
    }
}