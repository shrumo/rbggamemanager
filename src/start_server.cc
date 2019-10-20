//
// Created by shrum on 02.07.19.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

#include <asio/io_service.hpp>
#include <networking/server.h>

using namespace rbg;

int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <filename> <port> [<deadline_in_seconds>]\n";
    return 1;
  }

  double deadline_in_seconds = std::numeric_limits<double>::max();
  if (argc == 4) {
    deadline_in_seconds = std::stod(argv[3]); 
  }

  std::ifstream filestream(argv[1]);
  std::stringstream buffer;
  buffer << filestream.rdbuf();

  try {
    asio::io_service io_service;
    Server server(buffer.str(), static_cast<unsigned short>(std::atoi(argv[2])), deadline_in_seconds);
    server.Run();
  }
  catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
