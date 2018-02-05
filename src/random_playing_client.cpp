//
// Created by shrum on 01/02/18.
//

#include <iostream>
#include "networking/client.h"
#include "../rbgParser/src/game_items.hpp"
#include "game_components/game_description.h"
#include "parser_connection/parser_connection.h"
#include "game_components/game_state.h"

int main(int argc, char* argv[])
{
    /* TODO(shrum): Make proper program options with boost. */
    if (argc != 3)
    {
        std::cerr << "Usage: chat_server <host> <port>\n";
        return 1;
    }

    try
    {
        rbg_parser::messages_container msg;
        synchronous_client client(argv[argc - 2], argv[argc - 1]);
        std::stringstream buffer(client.description());

        std::cout << client.description() << std::endl;

        std::unique_ptr<rbg_parser::parsed_game> pg;
        try {
            std::vector<rbg_parser::token> result = tokenize(buffer.str(), msg);
            rbg_parser::game_items g = input_tokens(result, msg);
            pg = std::unique_ptr<rbg_parser::parsed_game>(new rbg_parser::parsed_game(g.parse_game(msg)));
        }catch(rbg_parser::message& m){
            std::cout<<"Game description is bad. Here is the error: " << "\n";
            std::cout<<m.as_error()<<std::endl;
            return 1;
        }
        game_description gd(create_description(*pg));

        search_context context;
        game_state state(gd);

        token_id_t assigned_player = gd.get_resolver().id(client.player());

        auto moves = state.find_moves(&context);
        while(!moves.empty()) {
            if (state.player() == assigned_player) {
                auto move = moves[rand() % moves.size()];
                state.make_move(move);
                client.write(move);
                moves = state.find_moves(&context);
            } else {
                auto move = client.read();
                state.make_move(move);
                moves = state.find_moves(&context);
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}