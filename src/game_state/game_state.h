//
// Created by shrum on 10.06.19.
//

#ifndef RBGGAMEMANAGER_GAME_STATE_H
#define RBGGAMEMANAGER_GAME_STATE_H

#include <game_state/construction/search_step_creator.h>
#include "board.h"
#include "declarations.h"

namespace rbg {

  using GameMove = std::vector<ModifierApplication>;

  struct MoveRevertInformation {
    AbstractBlock *previous_search_step;
    std::vector<ActionRevertInfo> player_steps_revert_information;
  };

  class GameState {
  public:
    friend class ModifyingSearchStep;

    friend class VisitedCheckTest;

    friend class ShiftAction;

    friend class OnTest;

    friend class OffApplication;

    friend class PlayerSwitchApplication;

    friend class AssignmentAction;

    friend class VariableValue;

    friend class PieceCountValue;

    friend class PlayerTest;

    friend class VisitedCheckTest;

    friend class VisitedCheckSearchStep;

    friend class ShiftStep;

    friend class OnStep;

    friend class OffStep;

    friend class PlayerSwitchStep;

    friend class AssignmentStep;

    friend class VariableValue;

    friend class PieceCountValue;

    friend class PlayerCheckStep;

    friend class RegisterModifierApplication;

    friend class ModifyingApplication;

    GameState(const Declarations &declarations, SearchStepsInformation steps)
        : declarations_(declarations), current_pos_(1),
          variables_values_(declarations.variables_resolver.size(), 0),
          current_player_(declarations.keeper_id),
          board_(declarations.board_description.initial_content()), steps_(std::move(steps)),
          moves_calculated(false) {
      steps_.current_position.SetParent(&steps_.collection);
      applied_modifiers_.reserve(10);
      moves_.reserve(10);
      std::vector<ActionRevertInfo> initial_keeper_step_revert_information;
      MakeKeeperClosure(initial_keeper_step_revert_information);
    }

    const std::vector<std::vector<ModifierApplication>>& Moves() {
      if(!moves_calculated) {
        steps_.current_position.current()->Run(this);
        moves_calculated = true;
        return moves_;
      }
      return moves_;
    }

    MoveRevertInformation Apply(const std::vector<ModifierApplication> &move) {
      moves_.clear();
      AbstractBlock *previous_search_step = steps_.current_position.current();
      std::vector<ActionRevertInfo> revert_infos;
      revert_infos.reserve(move.size());
      for (const auto &mod_application : move) {
        vertex_id_t previous_vertex = current_pos_;
        const ModifyingApplication* modifier = steps_.collection.Modifier(mod_application.modifier_index);
        current_pos_ = mod_application.vertex;
        revert_infos.push_back(ActionRevertInfo{modifier, previous_vertex, modifier->Apply(this)});
      }
      steps_.current_position.set_current(steps_.collection.Switch(move.back().modifier_index));
      steps_.collection.stack().reset();
      MakeKeeperClosure(revert_infos);
      moves_calculated=false;
      return {previous_search_step, revert_infos};
    }

    void Revert(const MoveRevertInformation &revert_info) {
      for (uint i = 0; i < revert_info.player_steps_revert_information.size(); i++) {
        const auto &step = revert_info.player_steps_revert_information[
            revert_info.player_steps_revert_information.size() - i - 1];
        step.application->Revert(this, step.revert_name);
        current_pos_ = step.vertex;
      }
      steps_.current_position.set_current(revert_info.previous_search_step);
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

    const SearchStepsInformation &search_steps() const {
      return steps_;
    }

    const std::vector<variable_value_t> &variables_values() const {
      return variables_values_;
    }

    void Reset() {
      current_pos_ = 1;
      variables_values_ = std::vector<variable_value_t >(declarations_.variables_resolver.size(), 0);
          current_player_ = declarations_.keeper_id;
          board_ = declarations_.board_description.initial_content();
          steps_.current_position.reset();
          steps_.collection.stack().reset();
          moves_calculated = false;
      applied_modifiers_.clear();
      applied_modifiers_.reserve(10);
      moves_.clear();
      moves_.reserve(10);
      std::vector<ActionRevertInfo> initial_keeper_step_revert_information;
      MakeKeeperClosure(initial_keeper_step_revert_information);
    }

  private:
    void MakeKeeperClosure(std::vector<ActionRevertInfo> &steps_revert_information) {
      bool exists = true;
      while (exists && current_player_ == declarations_.keeper_id) {
        applied_modifiers_.clear();
        moves_.clear();
        exists = steps_.current_position.current()->RunAndApplyFirst(this, &steps_revert_information, current_pos_);
        steps_.collection.stack().reset();
        if(exists) {
          steps_.current_position.set_current(steps_.collection.Switch(applied_modifiers_.back().modifier_index));
          steps_.collection.stack().reset();
          applied_modifiers_.clear();
        }
      }
    }


    const Declarations declarations_;
    vertex_id_t current_pos_;
    std::vector<variable_value_t> variables_values_;
    player_id_t current_player_;
    BoardContent board_;
    SearchStepsInformation steps_;
    std::vector<ModifierApplication> applied_modifiers_;
    std::vector<std::vector<ModifierApplication>> moves_;
    bool moves_calculated;
  };
}


#endif //RBGGAMEMANAGER_GAME_STATE_H
