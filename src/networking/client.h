//
// Created by shrum on 01/02/18.
//

#ifndef RBGGAMEMANAGER_SERVER_CLIENT_H
#define RBGGAMEMANAGER_SERVER_CLIENT_H


#include <boost/asio.hpp>
#include <iostream>
#include "../game_components/search_context.h"
#include "message.h"

using boost::asio::ip::tcp;

class synchronous_client {
    boost::asio::io_service io_service;
    tcp::resolver resolver;
    tcp::resolver::iterator endpoint_iterator;

    tcp::socket socket;

    std::string game_description;
    std::string assigned_player;

    void initialize()
    {
        message message = message::empty();
        boost::system::error_code ignored_error;
        boost::asio::read(socket, boost::asio::buffer(message.data_ptr(),header_length), ignored_error);
        message.decode_header();
        boost::asio::read(socket, boost::asio::buffer(message.body_ptr(),message.get_body_length()), ignored_error);
        game_description = std::string(message.body_ptr(),message.body_ptr() + message.get_body_length());
        boost::asio::read(socket, boost::asio::buffer(message.data_ptr(),header_length), ignored_error);
        message.decode_header();
        boost::asio::read(socket, boost::asio::buffer(message.body_ptr(),message.get_body_length()), ignored_error);
        assigned_player = std::string(message.body_ptr(),message.body_ptr() + message.get_body_length());
    }

public:
    synchronous_client(const char* host,const char* port)
        : resolver(io_service),
          endpoint_iterator(resolver.resolve({host,port})),
          socket(io_service)
    {
        boost::asio::connect(socket, endpoint_iterator);
        initialize();
    }

    void write(const move& move)
    {
        message message = message::client_move_message(move);
        boost::system::error_code ignored_error;
        boost::asio::write(socket, boost::asio::buffer(message.data_ptr(), message.length()), ignored_error);
    }

    const std::string& description() const
    {
        return game_description;
    }

    const std::string& player() const
    {
        return assigned_player;
    }

    move read()
    {
        message message = message::empty();
        boost::system::error_code ignored_error;
        boost::asio::read(socket, boost::asio::buffer(message.data_ptr(),header_length), ignored_error);
        message.decode_header();
        boost::asio::read(socket, boost::asio::buffer(message.body_ptr(),message.get_body_length()), ignored_error);
        return decode_move(message);
    }
};


#endif //RBGGAMEMANAGER_SERVER_CLIENT_H
