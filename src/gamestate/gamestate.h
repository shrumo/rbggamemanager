//
// Created by shrum on 10.06.19.
//

#ifndef RBGGAMEMANAGER_GAMESTATE_H
#define RBGGAMEMANAGER_GAMESTATE_H

#include <construction/searchstep_creator.h>
#include "board.h"
#include "declarations.h"

namespace rbg {

  class GameState {
  public:
    friend class ModifyingSearchStep;

    friend class VisitedCheckSearchStep;

    friend class ShiftStep;

    friend class OnStep;

    friend class OffStep;

    friend class PlayerSwitchStep;

    friend class AssignmentStep;

    friend class VariableValue;

    friend class PieceCountValue;

    GameState(const Declarations &declarations, SearchstepsCollection steps)
        : declarations_(declarations), current_pos_(0),
          variables_values_(declarations.variables_resolver.size(), 0),
          current_player_(declarations.keeper_id),
          board_(declarations.board_description), steps_(std::move(steps)) {
      steps_.stack().Push();
    }

    std::vector<std::vector<ModifierApplication>> Moves() {
      MakeKeeperClosure();
      std::vector<std::vector<ModifierApplication>> result;
      std::vector<ModifierApplication> modifiers;
      steps_.current()->Run(*this, modifiers, result);
      return result;
    }

    void MakeKeeperClosure() {
      while (current_player_ == declarations_.keeper_id) {
        steps_.current()->ApplyFirstFound(*this);
      }
    }

    void Apply(const std::vector<ModifierApplication> &move) {
      for (const auto &mod_application : move) {
        steps_.Modifier(mod_application.modifier_index)->Apply(*this, mod_application.vertex);
      }
      MakeKeeperClosure();
    }

    const Declarations &declarations() const {
      return declarations_;
    }

  private:
    const Declarations declarations_;
    vertex_id_t current_pos_;
    std::vector<variable_value_t> variables_values_;
    player_id_t current_player_;
    Board board_;
    SearchstepsCollection steps_;
  };
}


#endif //RBGGAMEMANAGER_GAMESTATE_H
