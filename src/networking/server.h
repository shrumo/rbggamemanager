//
// Created by shrum on 02.07.19.
//

#ifndef RBGGAMEMANAGER_SERVER_H
#define RBGGAMEMANAGER_SERVER_H


#include <asio.hpp>

#include <deque>

#include "socket.h"
#include <rbgParser/src/game_items.hpp>

#include <game_state/construction/game_state_creator.h>
#include <game_state/game_state.h>
#include <utility/printer.h>

namespace rbg {
  using asio::ip::tcp;

  class Server {
  public:
    explicit Server(const std::string &game_text, unsigned short port = 13)
        : io_service_{}, acceptor_(io_service_, tcp::endpoint(tcp::v4(), port)),
          state_(CreateGameState(game_text)), game_text_(game_text) {
      actions_translator_ = ActionsDescriptionsMap(game_text);
    }

    void Run() {
      while (clients_sockets_.size() != state_.declarations().players_resolver.size() - 1) {
        tcp::socket socket(io_service_);
        std::cout << "Waiting for clients..."
                  << "(" << state_.declarations().players_resolver.size() - 1 - clients_sockets_.size()
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
                  << state_.declarations().players_resolver.Name(client_player_id(i))
                  << "(" << client_player_id(i) << ")"
                  << " to client " << i << std::endl;
        clients_sockets_[i].WriteString(std::to_string(client_player_id(i)));
      }
      auto moves = state_.Moves();
      available_moves_ = std::unordered_set<GameMove>(moves.begin(), moves.end());
      while (!available_moves_.empty()) {


        std::cout << RectangularBoardDescription(state_.board_content(), state_.declarations()) << std::endl;
        std::cout << "Variables values are:" << std::endl;
        for (variable_id_t id = 0; id < state_.declarations().variables_resolver.size(); id++) {
          auto variable_name = state_.declarations().variables_resolver.Name(id);
          std::cout << "\t" << variable_name << " (" << id << ") : " << state_.variables_values()[id];
          if (state_.declarations().players_resolver.contains(variable_name)) {
            std::cout << " (result for player " << variable_name << " ("
                      << state_.declarations().players_resolver.Id(variable_name) << "))";
          }
          std::cout << std::endl;
        }

        std::cout << "Expecting move from " << player_socket_index(state_.current_player())
                  << " which is player "
                  << state_.declarations().players_resolver.Name(state_.current_player())
                  << " (" << state_.current_player() << ")" << std::endl;

        auto move = DecodeMove(clients_sockets_[player_socket_index(state_.current_player())].ReadString());

        std::cout << "Got a move from client " << player_socket_index(state_.current_player())
                  << " which is player "
                  << state_.declarations().players_resolver.Name(state_.current_player())
                  << " (" << state_.current_player() << "): "
                  << std::endl;

        for (const auto &mod : move) {
          std::cout << "\t" << state_.declarations().initial_board.vertices_names().Name(mod.vertex) << " ("
                    << mod.vertex << ") "
                    << actions_translator_[mod.modifier_index] << " (" << mod.modifier_index << ")" << std::endl;
        }

        if (available_moves_.find(move) == available_moves_.end()) {
          std::cout << "The move sent by the client is not legal" << std::endl;
          return;
        }

        for (uint i = 0; i < clients_sockets_.size(); i++) {
          if (i == player_socket_index(state_.current_player())) {
            continue;
          }
          std::cout << "Sending the move to " << i << " which is player "
                    << state_.declarations().players_resolver.Name(client_player_id(i))
                    << " (" << client_player_id(i) << ")"
                    << std::endl;
          clients_sockets_[i].WriteString(EncodeMove(move));
        }

        state_.Apply(move);

        moves = state_.Moves();
        available_moves_ = std::unordered_set<GameMove>(moves.begin(), moves.end());
      }

      std::cout << "Game ended." << std::endl;

      std::cout << RectangularBoardDescription(state_.board_content(), state_.declarations()) << std::endl;
      std::cout << "Variables values at end are:" << std::endl;
      for (variable_id_t id = 0; id < state_.declarations().variables_resolver.size(); id++) {
        auto variable_name = state_.declarations().variables_resolver.Name(id);
        std::cout << "\t" << variable_name << " (" << id << ") : " << state_.variables_values()[id];
        if (state_.declarations().players_resolver.contains(variable_name)) {
          auto player_id = state_.declarations().players_resolver.Id(variable_name);
          std::cout << " (result for " << player_socket_index(player_id) << " which is player " << variable_name << " ("
                    << player_id << "))";
        }
        std::cout << std::endl;
      }

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
  };
}


#endif //RBGGAMEMANAGER_SERVER_H
