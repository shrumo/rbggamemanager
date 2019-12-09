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

  struct ServerOptions {
    std::string game_text;
    unsigned short port = 8989;
    double deadline_seconds = std::numeric_limits<double>::max();
    double first_move_deadline_seconds = std::numeric_limits<double>::max();
    double shutdown_after_seconds = std::numeric_limits<double>::max();
    size_t shutdown_after_games = std::numeric_limits<size_t>::max();
    // The stream in which to store moves done in each game by each player.
    // If nullptr then they won't be stored.
    std::ostream *moves_logging_stream = nullptr;
    // The stream in which to store the results of each game. If nullptr then 
    // they won't be stored.
    std::ostream *results_logging_stream = nullptr;
  };

  class Server {
  public:
    explicit Server(ServerOptions options)
        : options_(std::move(options)), io_service_{}, acceptor_(io_service_, tcp::endpoint(tcp::v4(), options_.port)),
          state_(CreateGameState(options_.game_text)) {
      actions_translator_ = ActionsDescriptionsMap(options_.game_text);
    }

    void Run() {
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
        clients_sockets_[i].WriteString(options_.game_text);
        std::cout << "Sending player id "
                  << state_.declarations().players_resolver().Name(client_player_id(i))
                  << "(" << client_player_id(i) << ")"
                  << " to client " << i << std::endl;
        clients_sockets_[i].WriteString(std::to_string(client_player_id(i)));
      }

      auto play_start = std::chrono::system_clock::now();
      size_t games_count = 0;
      while (std::chrono::duration<double>(std::chrono::system_clock::now() - play_start).count() <
             options_.shutdown_after_seconds &&
             games_count < options_.shutdown_after_games) {
        size_t moves_done = 0;
        auto game_begin = std::chrono::system_clock::now();
        bool manager_deadline_exceeded = false;
        auto manager_begin = std::chrono::system_clock::now();
        auto moves = state_.Moves();
        available_moves_ = std::unordered_set<GameMove>(moves.begin(), moves.end());
        uint available_moves_overall = available_moves_.size();
        std::vector<bool> first_move(clients_sockets_.size(), true);
        bool exceeded_deadline = false;
        // Indexed py player socket index, gives out how many times this player socket exceeded deadline when sending move
        std::vector<int> deadlines_exceeded(clients_sockets_.size(), 0);
        // Indexed by player, gives out information whether the player timed out the first move.
        std::vector<bool> first_move_deadline_exceeded(clients_sockets_.size(), false);
        while (!available_moves_.empty()) {
          auto manager_end = std::chrono::system_clock::now();

          auto manager_duration = std::chrono::duration<double>(manager_end - manager_begin).count();
          if (manager_duration > options_.deadline_seconds) {
            std::cout << "Manager exceeded the deadline. The calculation took him " << manager_duration << "s."
                      << std::endl;
            exceeded_deadline = true;
            manager_deadline_exceeded = true;
          }
          auto move_string = clients_sockets_[player_socket_index(state_.current_player())].ReadString();
          auto end = std::chrono::system_clock::now();
          auto duration = std::chrono::duration<double>(end - manager_begin).count();

          if (first_move[player_socket_index(state_.current_player())]) {
            if (duration > options_.first_move_deadline_seconds) {
              std::cout << "First move deadline exceeded for client " << player_socket_index(state_.current_player())
                        << " which is player "
                        << state_.declarations().players_resolver().Name(state_.current_player())
                        << " (" << state_.current_player() << ") the move was received after " << duration << "s."
                        << std::endl;
              first_move_deadline_exceeded[player_socket_index(state_.current_player())] = true;
              deadlines_exceeded[player_socket_index(state_.current_player())]++;
              exceeded_deadline = true;
            }
          } else {
            if (duration > options_.deadline_seconds) {
              std::cout << "Deadline exceeded for client " << player_socket_index(state_.current_player())
                        << " which is player "
                        << state_.declarations().players_resolver().Name(state_.current_player())
                        << " (" << state_.current_player() << ") the move was received after " << duration << "s."
                        << std::endl;
              deadlines_exceeded[player_socket_index(state_.current_player())]++;
              exceeded_deadline = true;
            }
          }
          first_move[player_socket_index(state_.current_player())] = false;

          if (options_.moves_logging_stream != nullptr) {
            (*options_.moves_logging_stream) << move_string << "\n";
          }

          auto move = DecodeMove(move_string);

          if (available_moves_.find(move) == available_moves_.end()) {
            std::cout << "The move sent by the client is not legal" << std::endl;
            std::cout << "The move was: " << std::endl;
            for (const auto &mod : move) {
              std::cout << "\t" << state_.declarations().initial_board().vertices_names().Name(mod.vertex) << " ("
                        << mod.vertex
                        << ") "
                        << actions_translator_[mod.modifier_index] << " (" << mod.modifier_index << ")" << std::endl;
            }
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
          manager_begin = std::chrono::system_clock::now();
          moves = state_.Moves();
          available_moves_ = std::unordered_set<GameMove>(moves.begin(), moves.end());
          available_moves_overall += available_moves_.size();
        }

        auto game_end = std::chrono::system_clock::now();
        auto game_duration = std::chrono::duration<double>(game_end - game_begin).count();

        if (options_.results_logging_stream != nullptr) {
          auto &stream = *options_.results_logging_stream;
          stream << game_duration << " " << moves_done << " " << available_moves_overall << " ";
          for (uint i = 0; i < clients_sockets_.size(); i++) {
            if (i > 0) {
              stream << " ";
            }
            auto player_name = state_.declarations().players_resolver().Name(client_player_id(i));
            variable_id_t player_variable_id = state_.declarations().variables_resolver().Id(player_name);
            stream << state_.variables_values()[player_variable_id];
          }
          if (exceeded_deadline) {
            stream << " timeouts=";
            for (uint i = 0; i < clients_sockets_.size(); i++) {
              if (i > 0) {
                stream << ",";
              }
              stream << deadlines_exceeded[i];
              if (first_move_deadline_exceeded[i]) {
                stream << "(1)";
              }
            }
            if (manager_deadline_exceeded) {
              stream << ",manager";
            }
          }
          stream << std::endl;
        }
        state_.Reset();
        for (uint i = 0; i < clients_sockets_.size(); i++) {
          clients_sockets_[i].WriteString("reset");
        }
        if (options_.moves_logging_stream != nullptr) {
          (*options_.moves_logging_stream) << "reset" << std::endl;
        }
        games_count++;
        if (games_count % 200 == 0) {
          std::cout << games_count << " games were played so far." << std::endl;
        }
      }
      auto play_end = std::chrono::system_clock::now();
      std::cout << "Players played for " << std::chrono::duration<double>(play_end - play_start).count() << "s"
                << std::endl;
    }

  private:
    static player_id_t client_player_id(uint client_socket_index) {
      return client_socket_index + 1;
    }

    static uint player_socket_index(player_id_t player) {
      return player - 1;
    }

    ServerOptions options_;
    asio::io_service io_service_;
    tcp::acceptor acceptor_;
    GameState state_;
    std::unordered_set<GameMove> available_moves_;
    std::vector<StringSocket> clients_sockets_;
    std::unordered_map<uint, std::string> actions_translator_;
  };
}


#endif //RBGGAMEMANAGER_SERVER_H
