#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <array>
#include <string>
#include <iostream>

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

#include "game.h"


using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::resolver resolv{ioservice};
tcp::socket tcp_socket{ioservice};
std::array<char, 4096> bytes;

void read_handler(const boost::system::error_code &ec,
                  std::size_t bytes_transferred)
{
    if (!ec)
    {
        std::cout.write(bytes.data(), bytes_transferred);
        tcp_socket.async_read_some(buffer(bytes), read_handler);
    }
}

void connect_handler(const boost::system::error_code &ec)
{
    if (!ec)
    {
        std::string r =
                "GET / HTTP/1.1\r\nHost: theboostcpplibraries.com\r\n\r\n";
        write(tcp_socket, buffer(r));
        tcp_socket.async_read_some(buffer(bytes), read_handler);
    }
}

void resolve_handler(const boost::system::error_code &ec,
                     tcp::resolver::iterator it)
{
    if (!ec) {
        std::cout << (*it).service_name() << std::endl;
        tcp_socket.async_connect(*it, connect_handler);

    }
}

int main(int argc, char *argv[])
{
//    tcp::resolver::query q{"theboostcpplibraries.com", "80"};
//    resolv.async_resolve(q, resolve_handler);
//    ioservice.run();
    messages_container msg;
    std::string input_file_name(argv[argc-1]);
    std::ifstream t(input_file_name);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::vector<token> result = tokenize(buffer.str(),msg);
    game_items g = input_tokens(result,msg);
    parsed_game pg = g.parse_game(msg);
    game_description gd(pg);
}