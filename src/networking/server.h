//
// Create by shrum on 06.06.2020.
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

#include <mutex>

namespace rbg {
  using asio::ip::tcp;

  struct ServerOptions {
    std::string game_text;
    unsigned short port = 8989;
    // The number of seconds player have to make their move
    double deadline_seconds = std::numeric_limits<double>::max();
    // The number of seconds we give players for preparation. 
    double time_for_player_preparation = std::numeric_limits<double>::max() ;
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
        : options_(std::move(options)), io_context_{}, acceptor_(io_context_, tcp::endpoint(tcp::v4(), options_.port)),
          state_(CreateGameState(options_.game_text)) {
      actions_translator_ = ActionsDescriptionsMap(options_.game_text);
    }


    // Some things are synchronous, some asynchronous
    void Run() {
      { 
        std::lock_guard<std::mutex> guard(mutex_);
        // Wait for all the clients to connect
        while (clients_sockets_.size() != state_.declarations().players_resolver().size() - 1) {
          tcp::socket socket(io_context_);
          std::cout << "Waiting for clients..."
                    << "(" << state_.declarations().players_resolver().size() - 1 - clients_sockets_.size()
                    << " more needed)"
                    << std::endl;
          acceptor_.accept(socket);
          std::cout << "Got client number " << clients_sockets_.size() << std::endl;
          clients_sockets_.emplace_back(std::move(socket));
        }

        // Send the game text to clients
        for (uint i = 0; i < clients_sockets_.size(); i++) {
          std::cout << "Sending time for preparation to client " << i << std::endl; 
          clients_sockets_[i].WriteString(std::to_string(options_.time_for_player_preparation)); 
          std::cout << "Sending game text to client " << i << std::endl; 
          clients_sockets_[i].WriteString(options_.game_text); 
          std::cout << "Sending player id "
                     << state_.declarations().players_resolver().Name(client_player_id(i))
                     << "(" << client_player_id(i) << ")"
                     << " to client " << i << std::endl;
          clients_sockets_[i].WriteString(std::to_string(client_player_id(i)));
        }
      } // mutex unlock
      bool exceeded_initial_deadline = false;
      if(!WaitForReady()) {
        for(uint i = 0; i < clients_sockets_.size(); i++) {
            if(players_ready_.find(i) == players_ready_.end()) {
                std::cout << "Client " << i << " was late with sending ready." << std::endl;    
                exceeded_initial_deadline = true;
            }    
        }
      }
      {
        std::lock_guard<std::mutex> guard(mutex_);
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
          bool exceeded_deadline = exceeded_initial_deadline;
          // Indexed py player socket index, gives out how many times this player socket exceeded deadline when sending move
          std::vector<int> deadlines_exceeded(clients_sockets_.size(), 0);
          while (!available_moves_.empty()) {
            auto manager_end = std::chrono::system_clock::now();

            auto manager_duration = std::chrono::duration<double>(manager_end - manager_begin).count();
            if (manager_duration > options_.deadline_seconds) {
              std::cout << "Manager exceeded the deadline. The calculation took him " << manager_duration << "s."
                        << std::endl;
              exceeded_deadline = true;
              manager_deadline_exceeded = true;
            }
            clients_sockets_[player_socket_index(state_.current_player())].WriteString(std::to_string(options_.deadline_seconds)); 
            auto move_string = clients_sockets_[player_socket_index(state_.current_player())].ReadString();
            auto end = std::chrono::system_clock::now();
            auto duration = std::chrono::duration<double>(end - manager_begin).count();

            if (duration > options_.deadline_seconds) {
              std::cout << "Deadline exceeded for client " << player_socket_index(state_.current_player())
                        << " which is player "
                        << state_.declarations().players_resolver().Name(state_.current_player())
                        << " (" << state_.current_player() << ") the move was received after " << duration << "s."
                        << std::endl;
              deadlines_exceeded[player_socket_index(state_.current_player())]++;
              exceeded_deadline = true;
            }

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
                if (players_ready_.find(i) == players_ready_.end()) {
                    stream << "(lateready)";    
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
      } // mutex unlock
    }
 
   private:
    static player_id_t client_player_id(uint client_socket_index) {
     return client_socket_index + 1;
    }

    static uint player_socket_index(player_id_t player) {
      return player - 1;
    }

    // Await ready signal from all clients. 
    // Please do not hold the mutex while calling.
    bool WaitForReady() {
      {
      std::lock_guard<std::mutex> guard(mutex_); 
      auto time_sent = std::chrono::system_clock::now(); 
      // Send the deadline and the player ids
      for (uint i = 0; i < clients_sockets_.size(); i++) {
        clients_sockets_[i].AsyncReadString([this,i,time_sent](const asio::error_code& error, std::size_t size){HandleReady(size, i, error, time_sent);});
      } 
      } // unlock mutex for io_context to run
      io_context_.run();
      std::lock_guard<std::mutex> guard(mutex_);
      return players_ready_.size() == clients_sockets_.size();
    }

    void HandleReady(std::size_t message_size, uint player, const asio::error_code& error, std::chrono::time_point<std::chrono::system_clock> time_sent) {
      std::lock_guard<std::mutex> guard(mutex_);
      if (error || message_size != 6 /* size of string "ready" + null char */) {
        std::cout << "Something went wrong when receiving ready from player " << player << std::endl;
        return;
      }
      auto ready_received_time = std::chrono::system_clock::now();
      std::string players_message = clients_sockets_[player].ExtractNextStringFromBuffer();
      if (players_message != "ready") {
        std::cout << "Player sent a bad ready message. She sent " << players_message << " instead of ready. Treating it as ready." << std::endl;
      }
      std::cout << "Got ready from player " << player << std::endl; 
      if(std::chrono::duration<double>(ready_received_time - time_sent).count() < options_.time_for_player_preparation) { 
        players_ready_.insert(player);
      }
    }


    mutable std::mutex mutex_;

    const ServerOptions options_;
    asio::io_context io_context_;
    tcp::acceptor acceptor_;
    
    GameState state_;
    std::unordered_set<GameMove> available_moves_;
    std::vector<StringSocket> clients_sockets_;
    std::unordered_set<uint> players_ready_;
    std::unordered_map<uint, std::string> actions_translator_;   
  };

}

#endif
