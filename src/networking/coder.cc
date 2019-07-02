//
// Created by shrum on 02.07.19.
//

#include "coder.h"

std::string EncodeMove(const rbg::GameMove &move) {
  std::stringstream stream;
  for (size_t i = 0; i < move.size(); i++) {
    const auto &modifier_application = move[i];
    stream << modifier_application.vertex << " " << modifier_application.modifier_index;
    if (i != move.size() - 1)
      stream << " ";
  }
  return stream.str();
}

rbg::GameMove DecodeMove(const std::string &message) {
  std::stringstream stream(message);
  rbg::GameMove result;
  while (stream.peek() != std::char_traits<char>::eof()) {
    uint vertex, modifier_index;
    stream >> vertex >> modifier_index >> std::ws;
    result.push_back({vertex, modifier_index});
  }
  return result;
}
