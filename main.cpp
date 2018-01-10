#include <array>
#include <iostream>

#include "action.h"

#include<fstream>
#include<sstream>
#include <chrono>

#include"rbgParser/src/game_items.hpp"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

int main(int argc,const char **argv)
{
    po::options_description description("Allowed options");
    description.add_options()
            ("help","produce help message")
            ("human,hr","output game as it is meant to be read by human")
            ("time,t","output times for various stuff")
            ("scores,s","output scores at the end")
            ("moves,m","output number of turns and moves made")
            ("random,r","use a random player")
            ("input-file,i",po::value<std::string>(),"input file")
            ("randomseed",po::value<int>(),"random seed for random player")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc,argv,description), vm);
    po::notify(vm);

    po::positional_options_description p;
    p.add("input-file", -1);

    if(vm.count("randomseed"))
    {
        srand((unsigned int) vm["randomseed"].as<int>());
    }

    po::store(po::command_line_parser(argc, argv).
            options(description).positional(p).run(), vm);
    po::notify(vm);

    if(vm.count("help"))
    {
        std::cout << description << std::endl;
        return 1;
    }

    rbg_parser::messages_container msg;
    std::string input_file_name(vm["input-file"].as< std::string>());
    std::ifstream t(input_file_name);
    std::stringstream buffer;
    buffer << t.rdbuf();

    auto start = std::chrono::system_clock::now();
    std::vector<rbg_parser::token> result = tokenize(buffer.str(),msg);
    rbg_parser::game_items g = input_tokens(result,msg);
    rbg_parser::parsed_game pg = g.parse_game(msg);
    game_state state(pg);
    auto end = std::chrono::system_clock::now();
    if(vm.count("time"))
    {
        std::chrono::duration<double> elapsed = end-start;
        std::cout << "Game parsed in: " << elapsed.count() << "s" << std::endl;
    }
    if(vm.count("human")) {
        std::cout << "Game description: " << "\n";
        std::cout << pg.to_rbg(true) << std::endl;
    }
    std::vector<game_move> moves = state.find_all_moves();
    start = std::chrono::system_clock::now();
    while(!moves.empty()) {
        if(vm.count("human")) {
            std::cout << state << std::endl;
            std::cout << "Turn: " << state.sigma[state.get_name_resolver().get_variable_id("turn")] << std::endl;

            std::cout << "Player: " << state.get_name_resolver().get_player_name(state.current_player) << std::endl;
            std::cout << "Possible moves: " << std::endl;
            for (const game_move &m : moves) {
                for (const move_segment &ac : m.segments) {
                    std::cout << ac.board_x << " " << ac.board_y << " " << ac.modifier_index << ", ";
                }
                std::cout << std::endl;
            }
            std::cout << "Which move to make? [0 - " << moves.size() - 1 << "]" << std::endl;
        }
        size_t x = rand() % moves.size();
        if(!vm.count("random"))
            std::cin >> x;
        state.make_move(moves[x]);
        moves = state.find_all_moves();
    }
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end-start;
    if(vm.count("human"))
        std::cout << state << std::endl;
    if(vm.count("human") || vm.count("moves")) {
        std::cout << "Moves made: " << state.get_moves_made() << std::endl;
        std::cout << "Turns made: " << state.turn() << std::endl;
    }
    if(vm.count("human") || vm.count("scores"))
    {
        std::cout << "Scores: " << "\n";
        for (size_t i = 0; i < state.get_name_resolver().players_count(); i++) {
            std::string player_name = state.get_name_resolver().get_player_name(i);
            std::cout << "\t" << state.get_name_resolver().get_player_name(i) << " : "
                      << state.sigma[state.get_name_resolver().get_variable_id(player_name)] << "\n";
        }
    }
    if(vm.count("time")) {
        std::cout << "Game played in: " << elapsed.count() << "s" << std::endl;
    }
}