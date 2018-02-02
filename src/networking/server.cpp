//
// Created by shrum on 31/01/18.
//

#include "server.h"

void client_connection::do_write() {
    auto self(shared_from_this());
    boost::asio::async_write(socket,
                             boost::asio::buffer(write_moves.front().data_ptr(),
                                                 write_moves.front().length()),
                             [this, self](boost::system::error_code ec, std::size_t /*length*/)
                             {
                                 if (!ec)
                                 {
                                     write_moves.pop_front();
                                     if (!write_moves.empty())
                                     {
                                         do_write();
                                     }
                                 } else
                                 {
                                     instance->leave(shared_from_this());
                                 }
                             });
}

void client_connection::do_read_body() {
    auto self(shared_from_this());
    boost::asio::async_read(socket,
                            boost::asio::buffer(read_move.body_ptr(), read_move.get_body_length()),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/)
                            {
                                bool valid = instance->handle_move(shared_from_this(), read_move);
                                if (valid && !ec) {
                                    do_read_header();
                                } else
                                {
                                    instance->leave(shared_from_this());
                                }
                            });
}

void client_connection::do_read_header() {
    auto self(shared_from_this());
    boost::asio::async_read(socket,boost::asio::buffer(read_move.data_ptr(),header_length),
                            [this, self](boost::system::error_code ec, std::size_t /*length*/)
                            {
                                if (!ec && read_move.decode_header())
                                    do_read_body();
                                else
                                    instance->leave(shared_from_this());
                            });
}

void client_connection::start() {
    instance->join(shared_from_this());
    do_read_header();
}

void server_game_instance::leave(const client_connection::pointer &client) {
    /* TODO(shrum): Figure out what to do with other clients when someone leaves. */
    std::cout << client << " left the game." << std::endl;
    token_id_t player_token = clients[client];
    clients.erase(client);
    players[player_token] = nullptr;
    state.reset();
    if(controlling_server)
        controlling_server->do_accept();
}
