//
// Created by shrum on 02.07.19.
//

#ifndef RBGGAMEMANAGER_SOCKET_H
#define RBGGAMEMANAGER_SOCKET_H

#include <string>
#include <game_state/game_state.h>
#include <asio.hpp>

namespace rbg {
  using asio::ip::tcp;

  std::string EncodeMove(const rbg::GameMove &move);
  rbg::GameMove DecodeMove(const std::string &message);

  class StringSocket {
  public:
    explicit StringSocket(asio::io_service& service) : socket_(service) {}
    explicit StringSocket(tcp::socket socket) : socket_(std::move(socket)) {}

    StringSocket(StringSocket&& other) noexcept :
      socket_(std::move(other.socket_)), buffer{}
    {
      std::string content = std::string(asio::buffers_begin(buffer.data()),
                          asio::buffers_end(buffer.data()));
      buffer.sputn(content.data(), content.size());
    }

    std::string ReadString() {
      asio::read_until(socket_, buffer, '\0');
      std::string res;
      std::istream is(&buffer);
      std::getline(is, res, '\0');
      return res;
    }

    void WriteString(const std::string &data) {
      asio::write(socket_, asio::buffer(data.c_str(),
                                       data.length() + 1));
    }

    tcp::socket& socket() {
      return socket_;
    }

  private:
    tcp::socket socket_;
    asio::streambuf buffer;
  };
}

namespace std {
  template<>
  struct hash<rbg::ModifierApplication> {
    std::size_t operator()(const rbg::ModifierApplication &m) const {
      return (m.vertex) * 83 + m.modifier_index;;
    }
  };

  template<>
  struct hash<rbg::GameMove> {
    std::size_t operator()(const rbg::GameMove &m) const {
      std::size_t result = 0;
      for (const auto &mod_application : m) {
        result *= 2579;
        result += (mod_application.vertex) * 83 + mod_application.modifier_index;
      }
      return result;
    }
  };
}

#endif //RBGGAMEMANAGER_SOCKET_H
