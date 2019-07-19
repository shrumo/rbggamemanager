//
// Created by shrum on 02.07.19.
//

#include "server.h"

using namespace rbg;

void rbg::ClientConnection::Write() {
  auto self(shared_from_this());
  asio::async_write(socket_,
                    asio::buffer(write_moves_.front().c_str(),
                                 write_moves_.front().length()+1),
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

void rbg::ClientConnection::Read() {
  auto self(shared_from_this());
  asio::async_read_until(socket_,
                   read_move_, '\0',
                   [this, self](asio::error_code ec,
                                std::size_t /*length*/) {
                      std::string res;
                      std::istream is(&read_move_);
                     std::getline(is, res, '\0');
                     if(res.empty()) {
                       instance_->Leave(shared_from_this());
                       return;
                     }
                     bool valid = instance_->HandleMove(
                         shared_from_this(), res);
                     if (valid && !ec) {
                       Read();
                     } else {
                       instance_->Leave(shared_from_this());
                     }
                   });
}

void rbg::ClientConnection::Start() {
  instance_->Join(shared_from_this());
  Read();
}

void rbg::ServerGameInstance::Leave(const ClientConnection::pointer &client) {
  std::cout << client << " left_ the game." << std::endl;
  state_.Reset();
  player_id_t player_token = clients_[client];
  clients_.erase(client);
  players_[player_token] = nullptr;

  if (controlling_server_)
    controlling_server_->DoAccept();
}