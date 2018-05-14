//
// Created by shrum on 12.01.18.
//

#include <iomanip>
#include <cmath>
#include "game_state.h"

ActionResult
GameState::ApplyActionApplication(const ActionApplication &application) {
  vertex_t old_pos = current_pos_;
  current_pos_ = application.pos();
  auto res = application.action()->Apply(this);
  current_pos_ = old_pos;
  return res;
}

void
GameState::RevertActionApplication(const ActionApplication &application,
                                   const ActionResult &application_result) {
  vertex_t old_pos = current_pos_;
  current_pos_ = application.pos();
  application.action()->Revert(this, application_result);
  current_pos_ = old_pos;
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
  s << "Position: " << state.pos() << "[NFA state: "
    << state.nfa_state() << "]\n";
  auto edge_size = static_cast<int>(std::sqrt(state.board().size()));
  if(edge_size*edge_size != state.board().size())
  {
    edge_size = static_cast<int>(state.board().size());
  }
  for (vertex_t v = 0; v < state.board().size(); v++)
  {
    std::string name = state.description().resolver().Name(
        state.board()[v]);
    int width = 2;
    if (name == "empty" || name == "e") {
      s << "[" << std::setw(width) << " " << "] ";
    } else {
      s << "[" << std::setw(width) << name.substr(0, (unsigned long) width)
        << "] ";
    }
    if((v + 1) % edge_size == 0)
      s << '\n';
  }
  return s;
}
