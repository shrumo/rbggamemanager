//
// Created by shrum on 30.06.19.
//

#ifndef RBGGAMEMANAGER_GAME_STATE_PRINTER_H
#define RBGGAMEMANAGER_GAME_STATE_PRINTER_H

#include <string>
#include <sstream>
#include <cassert>
#include <iomanip>
#include "board.h"
#include "declarations.h"

namespace rbg {
  std::string RectangularBoardDescription(const BoardContent &board_content, const Declarations &declarations) {
    size_t width = 0;
    size_t height = 0;
    for (vertex_id_t v = 0; v < static_cast<ssize_t >(declarations.board_description.vertices_count()); v++) {
      std::string vertex_name = declarations.board_description.vertices_names().Name(v);
      std::stringstream stream(vertex_name);
      char placeholders[3] = {'\0', '\0', '\0'};
      stream >> placeholders[0] >> placeholders[1];
      assert(std::string(placeholders) == "rx" && "The board should be created by rectangular keyword.");
      size_t x, y;
      char placeholder;
      stream >> x >> placeholder >> y;
      assert(placeholder == 'y' && "The board should be created by rectangular keyword.");
      width = std::max(x + 1, width);
      height = std::max(y + 1, height);
    }
    std::vector<piece_id_t> field(width * height);
    for (vertex_id_t v = 0; v < static_cast<ssize_t >(declarations.board_description.vertices_count()); v++) {
      std::string vertex_name = declarations.board_description.vertices_names().Name(v);
      std::stringstream stream(vertex_name);
      char placeholder;
      stream >> placeholder >> placeholder;
      size_t x, y;
      stream >> x >> placeholder >> y;
      field[x + y * width] = board_content.at(v);
    }
    std::stringstream s;
    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
        std::string name = declarations.pieces_resolver.Name(field[x + y * width]);
        if (name == "empty" || name == "e") {
          s << "[" << std::setw(3) << " " << "] ";
        } else {
          s << "[" << std::setw(3) << name.substr(0, (unsigned long) 3)
            << "] ";
        }
      }
      s << '\n';
    }
    return s.str();
  }
}
#endif //RBGGAMEMANAGER_GAME_STATE_PRINTER_H
