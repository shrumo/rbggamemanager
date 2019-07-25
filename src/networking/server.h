//
// Created by shrum on 02.07.19.
//

#ifndef RBGGAMEMANAGER_SERVER_H
#define RBGGAMEMANAGER_SERVER_H


#include <asio.hpp>

#include <deque>

#include "coder.h"
#include <rbgParser/src/game_items.hpp>

#include <game_state/construction/game_state_creator.h>
#include <game_state/game_state.h>
#include <utility/printer.h>

namespace rbg {
  using asio::ip::tcp;

  class ServerGameInstance;

  class ClientConnection
      : public std::enable_shared_from_this<ClientConnection> {
  public:
    typedef std::shared_ptr<ClientConnection> pointer;

    static pointer Create(tcp::socket socket, ServerGameInstance *instance) {
      return pointer(new ClientConnection(std::move(socket), instance));
    }

    tcp::socket &socket() {
      return socket_;
    }

    void Start();

    void Send(const std::string &message) {
      bool write_in_progress = !write_moves_.empty();
      write_moves_.push_back(message);
      if (!write_in_progress) {
        Write();
      }
    }
  private:
    tcp::socket socket_;

    asio::streambuf read_move_;
    std::deque<std::string> write_moves_;

    ServerGameInstance *instance_;

    ClientConnection(tcp::socket socket, ServerGameInstance *instance) :
        socket_(std::move(socket)),
        instance_(instance) {}


    void Read();

    void Write();
  };

  class Server;

  class ServerGameInstance {
  public:
    explicit ServerGameInstance(const std::string &game_text,
                                Server *controlling_server = nullptr)
        : state_(CreateGameState(game_text)), controlling_server_(controlling_server), available_moves_{},
          game_text_(game_text) {
      players_ = std::vector<ClientConnection::pointer>(state_.declarations().players_resolver.size());
      for (player_id_t player_token = 0; player_token < state_.declarations().players_resolver.size(); player_token++) {
        players_[player_token] = nullptr;
      }
    }

    void Join(const ClientConnection::pointer &client) {
      std::lock_guard<std::mutex> lock(mutex_);
      std::cout << client << " joins the game." << std::endl;
      for (player_id_t player_token = 1; player_token < state_.declarations().players_resolver.size(); player_token++) {
        if (players_[player_token] == nullptr) {
          clients_[client] = player_token;
          players_[player_token] = client;
          std::cout << client << " is player "
                    << state_.declarations().players_resolver.Name(player_token)
                    << "." << std::endl;
          return;
        }
      }
      std::cout << client << " has no player assigned." << std::endl;
    }



    bool Full() const {
      std::lock_guard<std::mutex> lock(mutex_);
      return clients_.size() ==
             state_.declarations().players_resolver.size() - 1;
    }

    void KickAll();


    void Start() {
      std::lock_guard<std::mutex> lock(mutex_);
      std::cout << "Starting the game." << std::endl;
      for (const auto &pair : clients_) {
        const auto &client = pair.first;
        client->Send(game_text_);
        client->Send(std::to_string(pair.second));
      }
      auto moves = state_.Moves();
      available_moves_ = std::unordered_set<GameMove>(moves.begin(), moves.end());
      std::cout << RectangularBoardDescription(state_.board_content(), state_.declarations()) << std::endl;
      if (available_moves_.empty()) {
        Stop();
      }
    }

    bool
    HandleMove(const ClientConnection::pointer &client, const std::string &message) {
      std::lock_guard<std::mutex> lock(mutex_);
      player_id_t client_player_id = clients_[client];
      std::cout << "Got a move from client " << client
                << " which is player "
                << state_.declarations().players_resolver.Name(client_player_id)
                << std::endl;
      std::cout << message << std::endl;
      if (client_player_id != state_.current_player()) {
        std::cout << "Wrong player." << std::endl;
        return false;
      }
      auto move = DecodeMove(message);
      if (available_moves_.find(move) == available_moves_.end()) {
        std::cout << "Wrong move." << std::endl;
        return false;
      }
      state_.Apply(move);
      DeliverToEveryoneBut(message, client);
      auto moves = state_.Moves();
      available_moves_ = std::unordered_set<GameMove>(moves.begin(), moves.end());
      std::cout << RectangularBoardDescription(state_.board_content(), state_.declarations()) << std::endl;
      if (available_moves_.empty()) {
        Stop();
      }
      return true;
    }

  private:
    void DeliverToEveryoneBut(const std::string &msg, const ClientConnection::pointer &omit) {
      for (const auto &pair : clients_) {
        if (pair.first && pair.first != omit)
          pair.first->Send(msg);
      }
    }


    void Stop() {
      std::cout << "Game ended." << std::endl;
      for (const auto &pair : clients_) {
        std::string player_name =  state_.declarations().players_resolver.Name(pair.second);
        std::cout << "\t" << pair.second << " (" << player_name << ")"
                  << "(" << pair.first << ")" << " : "
                  << state_.variables_values()[state_.declarations().variables_resolver.Id(player_name)] << std::endl;
      }
    }

    mutable std::mutex mutex_;
    std::unordered_map<ClientConnection::pointer, player_id_t> clients_;
    std::vector<ClientConnection::pointer> players_;
    GameState state_;
    Server *controlling_server_;
    std::unordered_set<GameMove> available_moves_;
    std::string game_text_;
  };

  class Server {
  public:
    explicit Server(const std::string &game_text,
                    asio::io_service &io_service, unsigned short port = 13)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
          game_instance_(game_text, this), socket_(io_service) {
      DoAccept();
    }

    void DoAccept() {
      acceptor_.async_accept(socket_,
                             [this](asio::error_code ec) {
                               if (!ec && !game_instance_.Full()) {
                                 auto new_connection = ClientConnection::Create(
                                     std::move(socket_), &game_instance_);
                                 new_connection->Start();
                               }
                               if (!game_instance_.Full())
                                 DoAccept();
                               else
                                 game_instance_.Start();
                             });
    }

  private:
    tcp::acceptor acceptor_;
    ServerGameInstance game_instance_;

    tcp::socket socket_;
  };
}


#endif //RBGGAMEMANAGER_SERVER_H
