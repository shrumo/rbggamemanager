//
// Created by shrum on 02.07.19.
//

#ifndef RBGGAMEMANAGER_CLIENT_H
#define RBGGAMEMANAGER_CLIENT_H

#include <asio.hpp>
#include <iostream>
#include <game_state/game_state.h>
#include "socket.h"

namespace rbg {

  using asio::ip::tcp;

  class Client {

  public:
    Client(const char *host, const char *port)
        : resolver_(io_service_),
          endpoint_iterator_(resolver_.resolve({host, port})),
          string_socket_(io_service_),
          assigned_player_{0} {
      asio::connect(string_socket_.socket(), endpoint_iterator_);
      Initialize();
    }

    void Write(const rbg::GameMove &move) {
      string_socket_.WriteString(EncodeMove(move));
    }

    const std::string &description() const {
      return game_description_;
    }

    const player_id_t &player() const {
      return assigned_player_;
    }

    GameMove Read() {
      auto str = string_socket_.ReadString();
      return DecodeMove(str);
    }

  private:
    void Initialize() {
      game_description_ = string_socket_.ReadString();

      auto player_id_str = string_socket_.ReadString();
      assigned_player_ = std::atoi(player_id_str.c_str());
    }

    asio::io_service io_service_;
    tcp::resolver resolver_;
    tcp::resolver::iterator endpoint_iterator_;

    StringSocket string_socket_;

    std::string game_description_;
    player_id_t assigned_player_;
  };
}
#endif //RBGGAMEMANAGER_CLIENT_H
