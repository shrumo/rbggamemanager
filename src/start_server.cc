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
    std::cout << "Usage: " << argv[0] << " <filename> <port> [--deadline <deadline_time>]" << std::endl;
    return 0;
  }

  double deadline_in_seconds = std::numeric_limits<double>::max();
  if (args.flags.find("deadline") != args.flags.end()) {
    deadline_in_seconds = std::stod(args.flags.at("deadline")); 
  }

  std::unique_ptr<std::ofstream> logging_file;
  if (args.flags.find("log") != args.flags.end()) {
    logging_file = std::make_unique<std::ofstream>(args.flags.at("log"));
  }

  std::ifstream filestream(args.positional_args[0]);
  std::stringstream buffer;
  buffer << filestream.rdbuf();

  try {
    asio::io_service io_service;
    Server server(buffer.str(), static_cast<unsigned short>(std::stoi(args.positional_args[1])), deadline_in_seconds, logging_file.get());
    server.Run();
  }
  catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
