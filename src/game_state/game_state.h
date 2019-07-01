//
// Created by shrum on 10.06.19.
//

#ifndef RBGGAMEMANAGER_GAME_STATE_H
#define RBGGAMEMANAGER_GAME_STATE_H

#include <construction/search_step_creator.h>
#include "board.h"
#include "declarations.h"

namespace rbg {

  struct MoveRevertInformation {
    SearchStep *previous_search_step;
    std::vector<StepRevertInformation> player_steps_revert_information;
  };

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
      std::vector<StepRevertInformation> initial_keeper_step_revert_information;
      MakeKeeperClosure(initial_keeper_step_revert_information);
    }

    std::vector<std::vector<ModifierApplication>> Moves() {
      std::vector<std::vector<ModifierApplication>> result;
      std::vector<ModifierApplication> modifiers;
      steps_.current()->Run(*this, modifiers, result);
      steps_.stack().reset();
      return result;
    }

    MoveRevertInformation Apply(const std::vector<ModifierApplication> &move) {
      SearchStep *previous_search_step = steps_.current();
      std::vector<StepRevertInformation> revert_infos;
      revert_infos.reserve(move.size());
      for (const auto &mod_application : move) {
        revert_infos.push_back(
            steps_.Modifier(mod_application.modifier_index)->ApplyReversibleAtVertex(*this, mod_application.vertex));
      }
      steps_.set_current(revert_infos.back().modifying_step.next_step());
      steps_.stack().reset();
      MakeKeeperClosure(revert_infos);
      return {previous_search_step, revert_infos};
    }

    void Revert(const MoveRevertInformation &revert_info) {
      for (uint i = 0; i < revert_info.player_steps_revert_information.size(); i++) {
        const auto &step = revert_info.player_steps_revert_information[
            revert_info.player_steps_revert_information.size() - i - 1];
        step.modifying_step.RevertFromRevertInfo(*this, step);
      }
      steps_.set_current(revert_info.previous_search_step);
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
    void MakeKeeperClosure(std::vector<StepRevertInformation> &steps_revert_information) {
      bool exists = true;
      while (exists && current_player_ == declarations_.keeper_id) {
        exists = steps_.current()->ApplyFirstFound(*this, steps_revert_information, current_pos_);
        steps_.set_current(steps_revert_information.back().modifying_step.next_step());
        steps_.stack().reset();
      }
    }

    const Declarations declarations_;
    vertex_id_t current_pos_;
    std::vector<variable_value_t> variables_values_;
    player_id_t current_player_;
    BoardContent board_;
    SearchStepsInformation steps_;
  };
}


#endif //RBGGAMEMANAGER_GAME_STATE_H
