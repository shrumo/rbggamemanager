//
// Created by shrum on 02.07.19.
//

#ifndef RBGGAMEMANAGER_CODER_H
#define RBGGAMEMANAGER_CODER_H

#include <string>
#include <game_state/game_state.h>

std::string EncodeMove(const rbg::GameMove &move);

rbg::GameMove DecodeMove(const std::string &message);

namespace std {
  template<>
  struct hash<rbg::GameMove> {
    std::size_t operator()(const rbg::GameMove &m) const {
      std::size_t result = 0;
      for (const auto &mod_application : m) {
        result *= 2579;
        result += (mod_application.vertex) * 83 + mod_application.modifier_index;
      }
      return result;
    }
  };
}

#endif //RBGGAMEMANAGER_CODER_H
