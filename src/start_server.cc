//
// Created by shrum on 02.07.19.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

#include <asio/io_service.hpp>
#include <networking/server.h>
#include <stl_extension/argparse.h>

using namespace rbg;

int main(int argc, const char *argv[]) {
  auto args = std_ext::parse_args(argc, argv);

  if (args.positional_args.size() != 2) {
    std::cout << "Usage: " << argv[0]
              << " <filename> <port> [--deadline <deadline_time ms>] [--log_results <(result_logging_file|stdout)>] [--log_moves <(moves_logging_file|stdout)> [--shutdown <shutdown_time ms>] [--limit <games_limit>] [--time_for_player_preparation <preparation_time ms>]"
              << std::endl;
    return 0;
  }

  ServerOptions options;

  options.port = static_cast<unsigned short>(std::stoi(args.positional_args[1]));

  if (args.flags.find("deadline") != args.flags.end()) {
    options.deadline_seconds = std::stod(args.flags.at("deadline")) / 1000.0;
    options.time_for_player_preparation = options.deadline_seconds;
  }

  if (args.flags.find("time_for_player_preparation") != args.flags.end()) {
    options.time_for_player_preparation = std::stod(args.flags.at("time_for_player_preparation")) / 1000.0;
  }

  std::unique_ptr<std::ofstream> results_logging_file;
  if (args.flags.find("log_results") != args.flags.end()) {
    if(args.flags.at("log_results") != "stdout") {
      results_logging_file = std::make_unique<std::ofstream>(args.flags.at("log_results"));
      options.results_logging_stream = results_logging_file.get();
    }
    else {
      options.results_logging_stream = &std::cout;
    }
  }

  std::unique_ptr<std::ofstream> moves_logging_file;
  if (args.flags.find("log_moves") != args.flags.end()) {
    if(args.flags.at("log_moves") != "stdout") {
      moves_logging_file = std::make_unique<std::ofstream>(args.flags.at("log_moves"));
      options.moves_logging_stream = moves_logging_file.get();
    }
    else {
      options.moves_logging_stream = &std::cout;
    }
  }

  if (args.flags.find("shutdown") != args.flags.end()) {
    options.shutdown_after_seconds = std::stod(args.flags.at("shutdown")) / 1000.0;
  }

  if (args.flags.find("limit") != args.flags.end()) {
    options.shutdown_after_games = std::stoi(args.flags.at("limit"));
  }

  std::ifstream filestream(args.positional_args[0]);
  std::stringstream buffer;
  buffer << filestream.rdbuf();
  options.game_text = buffer.str();
  try {
    asio::io_service io_service;
    Server server(options); 
    server.Run();
  }
  catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
