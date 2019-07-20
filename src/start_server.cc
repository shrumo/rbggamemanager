//
// Created by shrum on 02.07.19.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <asio/io_service.hpp>
#include <networking/server.h>

using namespace rbg;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <filename> <port>\n";
    return 1;
  }

  std::ifstream filestream(argv[1]);
  std::stringstream buffer;
  buffer << filestream.rdbuf();

  std::string game_text = buffer.str();

  game_text = ParseGame(game_text)->to_rbg();

  try {
    asio::io_service io_service;
    Server server(game_text, io_service,
                  static_cast<unsigned short>(std::atoi(argv[argc - 1])));

    io_service.run();
  }
  catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}