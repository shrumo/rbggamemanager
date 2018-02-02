//
// Created by shrum on 31/01/18.
//

#ifndef RBGGAMEMANAGER_SERVER_H
#define RBGGAMEMANAGER_SERVER_H

#include <boost/asio.hpp>
#include <deque>
#include <fstream>
#include "message.h"
#include "../game_components/game_description.h"
#include "../game_components/game_state.h"
#include "../../rbgParser/src/game_items.hpp"
#include "../parser_connection/parser_connection.h"

using boost::asio::ip::tcp;

class server_game_instance;

class client_connection
    : public std::enable_shared_from_this<client_connection>
{
    tcp::socket socket;

    message read_move;
    std::deque<message> write_moves;

    server_game_instance* instance;

    client_connection(tcp::socket socket, server_game_instance* instance):
            socket(std::move(socket)), read_move(message::empty()),
            instance(instance)
    {}

    void do_read_header();
    void do_read_body();
    void do_write();

public:
    typedef std::shared_ptr<client_connection> pointer;

    static pointer create(tcp::socket socket, server_game_instance* instance)
    {
        return pointer(new client_connection(std::move(socket),instance));
    }

    tcp::socket& get_socket()
    {
        return socket;
    }

    void start();

    void send(const message& message)
    {
        bool write_in_progress = !write_moves.empty();
        write_moves.push_back(message);
        if(!write_in_progress)
        {
            do_write();
        }
    }
};

class server;

class server_game_instance
{
    std::unordered_map<client_connection::pointer, token_id_t> clients;
    std::unordered_map<token_id_t, client_connection::pointer> players;
    game_state state;
    server* controlling_server;
    std::unordered_set<move> available_moves;
    search_context context;

    void make_keeper_moves()
    {
        while(state.player() == state.get_description().get_deterministic_keeper_player_id() ||
              state.player() == state.get_description().get_nondeterministic_keeper_player_id())
        {
            if(state.player() == state.get_description().get_deterministic_keeper_player_id())
            {
                auto moves = state.find_moves(&context);
                if(moves.empty())
                {
                    stop();
                    return;
                }
                auto move = moves[rand()%moves.size()];
                state.make_move(move);
                deliver(message::client_move_message(move));
            }
            else if(state.player() == state.get_description().get_nondeterministic_keeper_player_id())
            {
                auto moves = state.find_first_move(&context);
                if(moves.empty())
                {
                    stop();
                    return;
                }
                state.make_move(moves[0]);
                deliver(message::client_move_message(moves[0]));
            }
        }
    }
public:
    explicit server_game_instance(const game_description& description,server* controlling_server=nullptr)
            : state(description), controlling_server(controlling_server)
    {
        for(token_id_t player_token : description.get_declarations().get_players_ids())
        {
            players[player_token] = nullptr;
        }
    }

    void join(const client_connection::pointer &client)
    {
        std::cout << client << " joins the game." << std::endl;
        for(token_id_t player_token : state.get_description().get_declarations().get_players_ids())
        {
            if(!players[player_token])
            {
                clients[client] = player_token;
                players[player_token] = client;
                std::cout<< client << " is player " << state.get_description().get_resolver().name(player_token)
                         << "."<< std::endl;
                if(full())
                    start();
                return;
            }
        }
        std::cout << client << " has no player assigned." << std::endl;
    }

    void start()
    {
        std::cout << "Starting the game." << std::endl;
        for(const auto& pair : clients)
        {
            const auto& client = pair.first;
            client->send(state.get_description().get_text_description());
            client->send(state.get_description().get_resolver().name(pair.second));
        }
        make_keeper_moves();
        auto moves = state.find_moves(&context);
        available_moves = std::unordered_set<move>(moves.begin(),moves.end());
        std::cout << state << std::endl;
        if(available_moves.empty())
        {
            stop();
        }
    }

    void stop()
    {
        std::cout << "Game ended." << std::endl;
        for(const auto& pair : clients)
        {
            std::cout << "\t" << state.get_description().get_resolver().name(pair.second)
                      << "(" << pair.first << ")" << " : " << state.value(pair.second) << std::endl;
        }
    }

    bool full() const
    {
        return clients.size() == state.get_description().get_declarations().get_players_ids().size();
    }

    void leave(const client_connection::pointer& client);

    void deliver(const message& msg)
    {
        for(const auto& pair : clients)
        {
            pair.first->send(msg);
        }
    }

    void deliver(const message& msg, const client_connection::pointer& omit)
    {
        for(const auto& pair : clients)
        {
            if(pair.first && pair.first != omit)
                pair.first->send(msg);
        }
    }

    bool handle_move(const client_connection::pointer &client, const message &message)
    {
        token_id_t client_player_id = clients[client];
        std::cout << "Got a move from client " << client
                  << " which is player " << state.get_description().get_resolver().name(client_player_id)
                  << std::endl;
        if(client_player_id != state.player())
        {
            std::cout << "Wrong player." << std::endl;
            return false;
        }
        auto move = decode_move(message);
        if(available_moves.find(move) == available_moves.end())
        {
            std::cout << "Wrong move." << std::endl;
            return false;
        }
        state.make_move(move);
        deliver(message, client);
        make_keeper_moves();
        auto moves = state.find_moves(&context);
        available_moves = std::unordered_set<move_type>(moves.begin(),moves.end());
        std::cout << state << std::endl;
        if(available_moves.empty())
        {
            stop();
        }
        return true;
    }
};

class server {
    tcp::acceptor acceptor;
    server_game_instance game_instance;

    tcp::socket socket_;
public:
    explicit server(const game_description& description, boost::asio::io_service& io_service, unsigned short port=13)
            : acceptor(io_service, tcp::endpoint(tcp::v4(),port)),
              game_instance(description,this), socket_(io_service)
    {
        do_accept();
    }

    void do_accept()
    {
        acceptor.async_accept(socket_,
                               [this](boost::system::error_code ec)
                               {
                                   if (!ec && !game_instance.full())
                                   {
                                       auto new_connection = client_connection::create(std::move(socket_), &game_instance);
                                       new_connection->start();
                                   }
                                   if(!game_instance.full())
                                       do_accept();
                               });
    }
};


#endif //RBGGAMEMANAGER_SERVER_H
