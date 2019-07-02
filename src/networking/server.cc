//
// Created by shrum on 02.07.19.
//

#include "server.h"

using namespace rbg;

void rbg::ClientConnection::Write() {
  auto self(shared_from_this());
  asio::async_write(socket_,
                    asio::buffer(write_moves_.front().data_ptr(),
                                 write_moves_.front().length()),
                    [this, self](asio::error_code ec,
                                 std::size_t /*length*/) {
                      if (!ec) {
                        write_moves_.pop_front();
                        if (!write_moves_.empty()) {
                          Write();
                        }
                      } else {
                        instance_->Leave(shared_from_this());
                      }
                    });
}

void rbg::ClientConnection::ReadBody() {
  auto self(shared_from_this());
  asio::async_read(socket_,
                   asio::buffer(read_move_.body_ptr(),
                                read_move_.body_length()),
                   [this, self](asio::error_code ec,
                                std::size_t /*length*/) {
                     bool valid = instance_->HandleMove(
                         shared_from_this(), read_move_);
                     if (valid && !ec) {
                       ReadHeader();
                     } else {
                       instance_->Leave(shared_from_this());
                     }
                   });
}

void rbg::ClientConnection::ReadHeader() {
  auto self(shared_from_this());
  asio::async_read(socket_, asio::buffer(read_move_.data_ptr(),
                                         kHeaderLength),
                   [this, self](asio::error_code ec,
                                std::size_t /*length*/) {
                     if (!ec && read_move_.DecodeHeader())
                       ReadBody();
                     else
                       instance_->Leave(shared_from_this());
                   });
}

void rbg::ClientConnection::Start() {
  instance_->Join(shared_from_this());
  ReadHeader();
}

void rbg::ServerGameInstance::Leave(const ClientConnection::pointer &client) {
  std::cout << client << " left_ the game." << std::endl;
  player_id_t player_token = clients_[client];
  clients_.erase(client);
  players_[player_token] = nullptr;

  if (controlling_server_)
    controlling_server_->DoAccept();
}