//
// Created by shrum on 10.06.19.
//

#ifndef RBGGAMEMANAGER_GAME_STATE_H
#define RBGGAMEMANAGER_GAME_STATE_H

#include <construction/search_step_creator.h>
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

    friend class PlayerCheckStep;

    GameState(const Declarations &declarations, SearchStepsInformation steps)
        : declarations_(declarations), current_pos_(0),
          variables_values_(declarations.variables_resolver.size(), 0),
          current_player_(declarations.keeper_id),
          board_(declarations.board_description.initial_content()), steps_(std::move(steps)) {
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

    const BoardContent &board_content() const {
      return board_;
    }

    vertex_id_t current_pos() const {
      return current_pos_;
    }

    player_id_t current_player() const {
      return current_player_;
    }

    const SearchStepsCollection &search_steps() const {
      return steps_;
    }

    const std::vector<variable_value_t> &variables_values() const {
      return variables_values_;
    }

  private:
    const Declarations declarations_;
    vertex_id_t current_pos_;
    std::vector<variable_value_t> variables_values_;
    player_id_t current_player_;
    BoardContent board_;
    SearchStepsInformation steps_;
  };
}


#endif //RBGGAMEMANAGER_GAME_STATE_H
