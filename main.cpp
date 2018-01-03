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
    for(const game_move& m : moves)
    {
        for(const move_segment& ac : m.segments)
        {
            std::cout << ac.board_x << " " << ac.board_y << " " << ac.modifier_index << ", ";
        }
        std::cout << std::endl;
    }
//    make_move(&state,moves[0]);
//    std::cout << "MOVE MADE" << std::endl;
//    moves = find_all_moves(&state);
//    for(const move& m : moves)
//    {
//        for(const modifier_action& ac : m.move_actions)
//        {
//            std::cout << ac.board_position_x << " " << ac.board_position_y << " " << ac.modifier_index << ", ";
//        }
//        std::cout << std::endl;
//    }
}