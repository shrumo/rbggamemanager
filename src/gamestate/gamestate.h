//
// Created by shrum on 10.06.19.
//

#ifndef RBGGAMEMANAGER_GAMESTATE_H
#define RBGGAMEMANAGER_GAMESTATE_H

#include "board.h"
#include "declarations.h"

namespace rbg {

  class GameState {
  public:
    friend class VisitedCheckSearchStep;

    friend class ShiftStep;

    friend class OnStep;

    friend class OffStep;

    friend class PlayerSwitchStep;

    friend class AssignmentStep;

    friend class VariableValue;

  private:
    Declarations declarations_;
    vertex_id_t current_pos_;
    std::vector<variable_value_t> variables_values_;
    player_id_t current_player_;
  };
}


#endif //RBGGAMEMANAGER_GAMESTATE_H
