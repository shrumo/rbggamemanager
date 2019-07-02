//
// Created by shrum on 02.07.19.
//

#ifndef RBGGAMEMANAGER_SYNCHRONOUS_CLIENT_H
#define RBGGAMEMANAGER_SYNCHRONOUS_CLIENT_H

#include <asio.hpp>
#include <iostream>
#include <game_state/game_state.h>
#include "message.h"
#include "coder.h"

namespace rbg {

  using asio::ip::tcp;

  class SynchronousClient {

  public:
    SynchronousClient(const char *host, const char *port)
        : resolver_(io_service_),
          endpoint_iterator_(resolver_.resolve({host, port})),
          socket_(io_service_) {
      asio::connect(socket_, endpoint_iterator_);
      Initialize();
    }

    void Write(const rbg::GameMove &move) {
      Message message = Message(EncodeMove(move));
      asio::error_code ignored_error;
      asio::write(socket_, asio::buffer(message.data_ptr(),
                                        message.length()),
                  ignored_error);
    }

    const std::string &description() const {
      return game_description_;
    }

    const player_id_t &player() const {
      return assigned_player_;
    }

    GameMove Read() {
      Message message;
      asio::error_code ignored_error;
      asio::read(socket_,
                 asio::buffer(message.data_ptr(), kHeaderLength),
                 ignored_error);
      message.DecodeHeader();
      asio::read(socket_, asio::buffer(message.body_ptr(),
                                       message.body_length()),
                 ignored_error);
      return DecodeMove(message.body_ptr());
    }

  private:
    void Initialize() {
      Message message;
      asio::error_code ignored_error;
      asio::read(socket_,
                 asio::buffer(message.data_ptr(), kHeaderLength),
                 ignored_error);
      message.DecodeHeader();
      asio::read(socket_, asio::buffer(message.body_ptr(),
                                       message.body_length()),
                 ignored_error);
      game_description_ = std::string(message.body_ptr(),
                                      message.body_ptr() + message.body_length());
      asio::read(socket_,
                 asio::buffer(message.data_ptr(), kHeaderLength),
                 ignored_error);
      message.DecodeHeader();
      asio::read(socket_, asio::buffer(message.body_ptr(),
                                       message.body_length()),
                 ignored_error);
      assigned_player_ = std::atoi(std::string(message.body_ptr(),
                                               message.body_ptr() + message.body_length()).c_str());
    }

    asio::io_service io_service_;
    tcp::resolver resolver_;
    tcp::resolver::iterator endpoint_iterator_;

    tcp::socket socket_;

    std::string game_description_;
    player_id_t assigned_player_;
  };
}
#endif //RBGGAMEMANAGER_SYNCHRONOUS_CLIENT_H
