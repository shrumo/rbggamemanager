//
// Created by shrum on 02.07.19.
//

#ifndef RBGGAMEMANAGER_SERVER_H
#define RBGGAMEMANAGER_SERVER_H


#include <asio.hpp>

#include <chrono>
#include <deque>
#include <limits>
#include <ostream>

#include "socket.h"
#include <rbgParser/src/game_items.hpp>

#include <game_state/construction/game_state_creator.h>
#include <game_state/game_state.h>
#include <utility/printer.h>

namespace rbg {
  using asio::ip::tcp;

  class Server {
  public:
    explicit Server(const std::string &game_text, 
                    unsigned short port = 13, 
                    double deadline_seconds=std::numeric_limits<double>::max(),
                    std::ostream *logging_stream = nullptr
                    )
        : io_service_{}, acceptor_(io_service_, tcp::endpoint(tcp::v4(), port)),
          state_(CreateGameState(game_text)), game_text_(game_text), 
          deadline_seconds_(deadline_seconds), logging_stream_(logging_stream) {
      actions_translator_ = ActionsDescriptionsMap(game_text);
    }

    void Run(double shutdown_timer=std::numeric_limits<double>::max(), std::size_t shutdown_after_games=std::numeric_limits<std::size_t>::max()) {
      while (clients_sockets_.size() != state_.declarations().players_resolver().size() - 1) {
        tcp::socket socket(io_service_);
        std::cout << "Waiting for clients..."
                  << "(" << state_.declarations().players_resolver().size() - 1 - clients_sockets_.size()
                  << " more needed)"
                  << std::endl;
        acceptor_.accept(socket);
        std::cout << "Got client number " << clients_sockets_.size() << std::endl;
        clients_sockets_.emplace_back(std::move(socket));
      }
      for (uint i = 0; i < clients_sockets_.size(); i++) {
        std::cout << "Sending game text to client " << i << std::endl;
        clients_sockets_[i].WriteString(game_text_);
        std::cout << "Sending player id "
                  << state_.declarations().players_resolver().Name(client_player_id(i))
                  << "(" << client_player_id(i) << ")"
                  << " to client " << i << std::endl;
        clients_sockets_[i].WriteString(std::to_string(client_player_id(i)));
      }

      auto play_start = std::chrono::system_clock::now();
      size_t games_count = 0;
      while(std::chrono::duration<double>(std::chrono::system_clock::now() - play_start).count() < shutdown_timer &&
            games_count < shutdown_after_games) {
        size_t moves_done = 0;
        auto game_begin = std::chrono::system_clock::now();

        auto moves = state_.Moves();
        available_moves_ = std::unordered_set<GameMove>(moves.begin(), moves.end());
        uint available_moves_overall = available_moves_.size();
        while (!available_moves_.empty()) {
          auto begin = std::chrono::system_clock::now();
          auto move_string = clients_sockets_[player_socket_index(state_.current_player())].ReadString(); 
          auto end = std::chrono::system_clock::now();
          auto duration = std::chrono::duration<double>(end - begin).count();

          if(duration > deadline_seconds_) {
                  std::cout << "Deadline exceeded for client " << player_socket_index(state_.current_player()) 
                            << " which is player " 
                            <<  state_.declarations().players_resolver().Name(state_.current_player())
                    << " (" << state_.current_player() << ")" << std::endl;
                  return;
          }

          auto move = DecodeMove(move_string);

          if (available_moves_.find(move) == available_moves_.end()) {
            std::cout << "The move sent by the client is not legal" << std::endl;
            return;
          }

          for (uint i = 0; i < clients_sockets_.size(); i++) {
            if (i == player_socket_index(state_.current_player())) {
              continue;
            }
            clients_sockets_[i].WriteString(EncodeMove(move));
          }

          state_.Apply(move);
          moves_done++;

          moves = state_.Moves();
          available_moves_ = std::unordered_set<GameMove>(moves.begin(), moves.end());
          available_moves_overall += available_moves_.size();
        }
        
        auto game_end = std::chrono::system_clock::now();
        auto game_duration = std::chrono::duration<double>(game_end - game_begin).count();
        
        if(logging_stream_ != nullptr) {
          auto& stream = *logging_stream_;
          stream << game_duration << " " << moves_done << " " << available_moves_overall << " ";
          for (uint i = 0; i < clients_sockets_.size(); i++) {
            if (i>0) {
              stream << " ";
            }
            auto player_name = state_.declarations().players_resolver().Name(client_player_id(i));
            variable_id_t player_variable_id = state_.declarations().variables_resolver().Id(player_name);
            stream << state_.variables_values()[player_variable_id]; 
          }
          stream << std::endl;
        }
        state_.Reset();
        for (uint i = 0; i < clients_sockets_.size(); i++) {
            clients_sockets_[i].WriteString("reset");
          }
        games_count++;
        if (games_count % 200 == 0) {
          std::cout << games_count << " games were played so far." << std::endl;
        }
      }
      auto play_end = std::chrono::system_clock::now();
      std::cout << "Players played for " << std::chrono::duration<double>(play_end - play_start).count() << "s" << std::endl; 
    }

  private:
    static player_id_t client_player_id(uint client_socket_index) {
      return client_socket_index + 1;
    }

    static uint player_socket_index(player_id_t player) {
      return player - 1;
    }

    asio::io_service io_service_;
    tcp::acceptor acceptor_;
    GameState state_;
    std::string game_text_;
    std::unordered_set<GameMove> available_moves_;
    std::vector<StringSocket> clients_sockets_;
    std::unordered_map<uint, std::string> actions_translator_;
    double deadline_seconds_;
    std::ostream *logging_stream_;
  };
}


#endif //RBGGAMEMANAGER_SERVER_H
