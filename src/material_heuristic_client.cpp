//
// Created by shrum on 01/02/18.
//

#include <iostream>
#include <chrono>
#include <iomanip>
#include "networking/client.h"
#include "../rbgParser/src/game_items.hpp"
#include "game_components/game_description.h"
#include "parser_connection/parser_connection.h"
#include "game_components/game_state.h"
#include "utils/utilities.h"

using namespace std;

static void collect_move_usages(token_id_t previous_player, const game_state &s, const move_type &m, vector<vector<size_t> > &result) {
    token_id_t player = s.player();
    if(player == s.get_description().get_deterministic_keeper_player_id() ||
            player == s.get_description().get_nondeterministic_keeper_player_id())
    {
        player = previous_player;
    }
    if(player == s.get_description().get_deterministic_keeper_player_id() ||
       player == s.get_description().get_nondeterministic_keeper_player_id())
    {
        return;
    }
    for (const auto &block : m.get_blocks())
    {
        for (const auto &action : get_actions_in_block(s.get_description().get_moves_description(),
                                                       block.get_block_id())) {
            if (action->get_type() == action_type::OFF_T) {
                auto *off_action = dynamic_cast<const actions::off *>(action);
                token_id_t piece = off_action->get_piece();
                result[piece][player]++;
            }
        }
    }
}

static vector<vector<size_t> > pieces_player_usages(const game_description &gd, size_t iters = 10) {
    vector<vector<size_t> > result(gd.get_resolver().names_count(),
                                   vector<size_t>(gd.get_resolver().names_count(),0));

    search_context context;
    token_id_t previous_player = gd.get_deterministic_keeper_player_id();
    for (size_t i = 0; i < iters; i++) {
        game_state s(gd);
        auto moves = s.find_moves(&context);
        for (const auto &move : moves)
            collect_move_usages(previous_player, s, move, result);
        while (!moves.empty()) {
            previous_player = s.player();
            s.make_move(moves[rand() % moves.size()]);
            moves = s.find_moves(&context);
            for (const auto &move : moves)
                collect_move_usages(previous_player, s, move, result);
        }
    }
    return result;
}

struct perft_result
{
    size_t leaf_count;
    size_t node_count;
};

static perft_result perft(search_context* context, game_state *state, size_t depth)
{
    if(depth == 0)
        return {1,1};

    std::vector<move_type> moves;
    size_t new_depth = depth - 1;
    if(state->player() == state->get_description().get_deterministic_keeper_player_id())
    {
        moves = state->find_first_move(context);
        new_depth = depth;
    } else {
        moves = state->find_moves(context);
    }
    size_t leaf_count = 0;
    size_t node_count = 1;
    for(const auto& move : moves) {
        auto revert_info = state->make_revertible_move(move);
        auto rec_res = perft(context, state, new_depth);
        leaf_count += rec_res.leaf_count;
        node_count += rec_res.node_count;
        state->revert_move(revert_info);
    }
    return {leaf_count, node_count};
}

static size_t estimate_depth(const game_description& gd, double secs)
{
    search_context c;
    game_state state(gd);
    size_t depth = 0;

    double duration = 0;
    do {
        depth++;
        auto begin = std::chrono::system_clock::now();
        volatile auto result = perft(&c, &state, depth);
        auto end = std::chrono::system_clock::now();
        duration = std::chrono::duration<double>(end - begin).count();
    } while(duration < secs);
    return depth-1;
}

/* TODO(shrum): make easy interface for move selectors and write more of them.*/
class material_heuristic_selector {

    token_id_t player;
    vector<vector<double> > probabilities;
    search_context c;
    size_t alpha_beta_depth;

    void calculate_owner_probability(const game_description &gd, size_t iters = 10) {

        auto pieces_player_uses = pieces_player_usages(gd, iters);
        for (auto piece : gd.get_declarations().get_pieces_ids()) {
            size_t all_uses = 0;
            for (auto player : gd.get_declarations().get_players_ids()) {
                all_uses += pieces_player_uses[piece][player];
            }
            for (auto player : gd.get_declarations().get_players_ids()) {
                probabilities[player][piece] = (double) pieces_player_uses[piece][player] / all_uses;
                probabilities[player][piece] = std::max(0.0, probabilities[player][piece]);
            }
        }
    }

    double heu(token_id_t player, game_state *s, const vector<vector<double> > &pieces_owner_probability) {
        double result = 0;
        for (const auto &piece : s->get_description().get_declarations().get_pieces_ids()) {
            result += s->value(piece) * pieces_owner_probability[player][piece];
        }
        return result;
    }

    double alphabeta(game_state *s, size_t depth, double alpha, double beta) {
        if (depth == 0)
            return heu(player, s, probabilities);
        auto moves = s->find_moves(&c);
        if (moves.empty())
            return heu(player, s, probabilities);
        if (player == s->player()) {
            double value = numeric_limits<double>::min();
            for (auto &move : moves) {
                auto r = s->make_revertible_move(move);
                value = std::max(value, alphabeta(s, depth - 1, alpha, beta));
                alpha = std::max(alpha, value);
                s->revert_move(r);
                if (beta <= alpha)
                    break;
            }
            return value;
        } else {
            double value = numeric_limits<double>::max();
            for (auto &move : moves) {
                auto r = s->make_revertible_move(move);
                value = std::min(value, alphabeta(s, depth - 1, alpha, beta));
                beta = std::min(beta, value);
                s->revert_move(r);
                if (beta <= alpha)
                    break;

            }
            return value;
        }
    }

public:
    material_heuristic_selector(const game_description &d, token_id_t player, size_t alpha_beta_depth)
            : player(player),
              probabilities(d.get_resolver().names_count(),
                            vector<double>(d.get_resolver().names_count())),
              alpha_beta_depth(alpha_beta_depth) {
        calculate_owner_probability(d);
    }

    move_type select(game_state *s, const vector<move_type> &moves) {
        move_type best;
        double best_value = numeric_limits<double>::min();
        for (const auto &move : moves) {
            auto r = s->make_revertible_move(move);
            double value = alphabeta(s, alpha_beta_depth, -numeric_limits<double>::min(),
                                     numeric_limits<double>::max());
            s->revert_move(r);
            if (value > best_value) {
                best_value = value;
                best = move;
            }
        }
        return best;
    }

    const vector<vector<double>> &get_probabilities() const
    {
        return probabilities;
    }
};

int main(int argc, char* argv[])
{
    /* TODO(shrum): Make proper program options with boost. */
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] <<" <host> <port>\n";
        return 1;
    }

    try
    {
        rbg_parser::messages_container msg;
        synchronous_client client(argv[argc - 2], argv[argc - 1]);
        std::stringstream buffer(client.description());

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

        size_t depth = estimate_depth(gd, 1.0);
        std::cout << "Will count minmax to depth: " << depth << std::endl;
        material_heuristic_selector selector(gd, assigned_player, depth);

        std::cout << "Ownerships: " << std::endl;
        std::cout << setw(5) << " " << " ";
        for(const auto piece : gd.get_declarations().get_pieces_ids())
        {
            std::cout << setw(5) << gd.get_resolver().name(piece).substr(0,5) << " ";
        }
        std::cout << "\n";
        for(const auto player : gd.get_declarations().get_players_ids())
        {
            std::cout << setw(5) << gd.get_resolver().name(player).substr(0,5) << " ";
            for(const auto piece : gd.get_declarations().get_pieces_ids())
            {
                std::cout << setw(5) << setprecision(2) << selector.get_probabilities()[player][piece] << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "I am player: " << gd.get_resolver().name(assigned_player) << std::endl;

        auto moves = state.find_moves(&context);
        while(!moves.empty()) {
            if (state.player() == assigned_player) {
                auto move = selector.select(&state, moves);
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