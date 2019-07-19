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
      WriteStringToSocket(EncodeMove(move));
    }

    const std::string &description() const {
      return game_description_;
    }

    const player_id_t &player() const {
      return assigned_player_;
    }

    GameMove Read() {
      auto str = ReadStringFromSocket();
      return DecodeMove(str.c_str());
    }

  private:
    std::string ReadStringFromSocket() {
      asio::streambuf b;
      asio::read_until(socket_, b,'\0');
      std::string res;
          std::istream is(&b);
        std::getline(is, res, '\0');
      return res;
    }

    void WriteStringToSocket(const std::string& data) {
       asio::write(socket_, asio::buffer(data.c_str(),
                                        data.length()+1));
    }

    void Initialize() {
      Message message;
      asio::error_code ignored_error;

      auto str = ReadStringFromSocket();
      game_description_ = str.c_str();


      str = ReadStringFromSocket();
      assigned_player_ = std::atoi(str.c_str());
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
