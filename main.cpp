//#include <boost/asio/io_service.hpp>
//#include <boost/asio/write.hpp>
//#include <boost/asio/buffer.hpp>
//#include <boost/asio/ip/tcp.hpp>
#include <array>
#include <string>
#include <iostream>

#include "action.h"

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include <chrono>

#include"rbg2gdl/src/options.hpp"
#include"rbg2gdl/src/token.hpp"
#include"rbg2gdl/src/message.hpp"
#include"rbg2gdl/src/game_items.hpp"
#include"rbg2gdl/src/macro_bank.hpp"
#include"rbg2gdl/src/slice_iterator.hpp"

#include "game_state.h"


//using namespace boost::asio;
//using namespace boost::asio::ip;
//
//io_service ioservice;
//tcp::resolver resolv{ioservice};
//tcp::socket tcp_socket{ioservice};
//std::array<char, 4096> bytes;
//
//void read_handler(const boost::system::error_code &ec,
//                  std::size_t bytes_transferred)
//{
//    if (!ec)
//    {
//        std::cout.write(bytes.data(), bytes_transferred);
//        tcp_socket.async_read_some(buffer(bytes), read_handler);
//    }
//}
//
//void connect_handler(const boost::system::error_code &ec)
//{
//    if (!ec)
//    {
//        std::string r =
//                "GET / HTTP/1.1\r\nHost: theboostcpplibraries.com\r\n\r\n";
//        write(tcp_socket, buffer(r));
//        tcp_socket.async_read_some(buffer(bytes), read_handler);
//    }
//}
//
//void resolve_handler(const boost::system::error_code &ec,
//                     tcp::resolver::iterator it)
//{
//    if (!ec) {
//        std::cout << (*it).service_name() << std::endl;
//        tcp_socket.async_connect(*it, connect_handler);
//
//    }
//}

int main(int argc, char *argv[])
{
//    tcp::resolver::query q{"theboostcpplibraries.com", "80"};
//    resolv.async_resolve(q, resolve_handler);
//    ioservice.run();
    rbg_parser::messages_container msg;
    std::string input_file_name(argv[argc-1]);
    std::ifstream t(input_file_name);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::vector<rbg_parser::token> result = tokenize(buffer.str(),msg);
    rbg_parser::game_items g = input_tokens(result,msg);
    rbg_parser::parsed_game pg = g.parse_game(msg);
    game_state state(pg);
    std::cout << pg.to_rbg(true) << std::endl;
    std::vector<game_move> moves = state.find_all_moves();
    auto start = std::chrono::system_clock::now();
    while(!moves.empty()) {
        std::cout << state << std::endl;
        std::cout << "Turn: " << state.sigma[state.get_name_resolver().get_variable_id("turn")] << std::endl;

        if(state.turn() == 22)
            std::cout<<"WAIT" <<std::endl;
        std::cout << "Player: " << state.get_name_resolver().get_player_name(state.current_player) << std::endl;
        std::cout << "Possible moves: " << std::endl;
        for (const game_move &m : moves) {
            for (const move_segment &ac : m.segments) {
                std::cout << ac.board_x << " " << ac.board_y << " " << ac.modifier_index << ", ";
            }
            std::cout << std::endl;
        }
        std::cout << "Which move to make? [0 - " << moves.size()-1 << "]" << std::endl;
        size_t x = rand() % moves.size();
//        std::cin >> x;
        state.make_move(moves[x]);
        moves = state.find_all_moves();
    }
    std::cout << state << std::endl;
    for(size_t i = 0; i < state.get_name_resolver().players_count() ; i++)
    {
        std::string player_name = state.get_name_resolver().get_player_name(i);
        std::cout << "Player " << state.get_name_resolver().get_player_name(i) << " scored " << state.sigma[state.get_name_resolver().get_variable_id(player_name)] << std::endl;
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end-start;
    std::cout << elapsed.count() << "s" << std::endl;
}