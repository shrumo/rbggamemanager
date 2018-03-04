//
// Created by shrum on 12.01.18.
//

#include <iomanip>
#include "game_state.h"

ActionResult
GameState::ApplyActionApplication(const ActionApplication &application) {
  std::size_t old_x = current_x_;
  std::size_t old_y = current_y_;
  current_x_ = application.x();
  current_y_ = application.y();
  auto res = application.action()->Apply(this);
  current_x_ = old_x;
  current_y_ = old_y;
  return res;
}

void
GameState::RevertActionApplication(const ActionApplication &application,
                                   const ActionResult &application_result) {
  std::size_t old_x = current_x_;
  std::size_t old_y = current_y_;
  current_x_ = application.x();
  current_y_ = application.y();
  application.action()->Revert(this, application_result);
  current_x_ = old_x;
  current_y_ = old_y;
}

std::ostream &operator<<(std::ostream &s, const GameState &state) {
  s << "Player: " << state.description().resolver().Name(state.player())
    << "\n";
  s << "Variables: ";
  for (token_id_t i = 0; i < state.description().VariablesCount(); i++) {
    s << state.description().resolver().Name(i) << ":" << state.Value(i);
    if (i != state.description().VariablesCount() - 1)
      s << ", ";
  }
  s << "\n";
  s << "Position: " << state.x() << ", " << state.y() << "[NFA state: "
    << state.nfa_state() << "]\n";
  for (size_t y = 0; y < state.height(); y++) {
    for (size_t x = 0; x < state.width(); x++) {
      std::string name = state.description().resolver().Name(
          state.board()(x, y));
      int width = 2;
      if (name == "empty") {
        s << "[" << std::setw(width) << " " << "] ";
      } else {
        s << "[" << std::setw(width) << name.substr(0, (unsigned long) width)
          << "] ";
      }
    }
    if (y != state.height() - 1)
      s << "\n";
  }
  return s;
}
