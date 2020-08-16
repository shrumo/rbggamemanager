//
// Created by shrum on 10.06.19.
//

#ifndef RBGGAMEMANAGER_GAME_STATE_H
#define RBGGAMEMANAGER_GAME_STATE_H

#include <game_state/construction/blocks_creator.h>
#include "game_description/board.h"
#include "game_description/declarations.h"

namespace rbg {

  using GameMove = std::vector<ModifierApplication>;

  struct MoveRevertInformation {
    Block *previous_search_step;
    std::vector<ActionRevertInfo> player_steps_revert_information;
  };

  class GameState {
  public:

    friend class VisitedCheckTest;

    friend class ShiftAction;

    friend class OnTest;

    friend class OffApplication;

    friend class PlayerSwitchApplication;

    friend class AssignmentAction;

    friend class VariableValue;

    friend class PieceCountValue;

    friend class PlayerTest;

    friend class ConditionVisitedCheckTest;

    friend class NegatedConditionCheckTest;

    friend class ConditionCheckTest;

    friend class VisitedCheckTest;

    friend class VariableValue;

    friend class PieceCountValue;

    friend class ModifyingApplication;

    GameState(const Declarations &declarations, BlocksCollection steps)
        : declarations_(declarations), current_pos_(1),
          variables_values_(declarations.variables_resolver().size(), 0),
          current_player_(declarations.keeper_id()),
          board_(declarations.initial_board().initial_content()), blocks_(std::move(steps)),
          current_block_(blocks_.initial()),
          moves_calculated(false) {
      applied_modifiers_.reserve(10);
      moves_.reserve(10);
      std::vector<ActionRevertInfo> initial_keeper_step_revert_information;
      MakeKeeperClosure(initial_keeper_step_revert_information);
    }

    const std::vector<std::vector<ModifierApplication>> &Moves() {
      if (!moves_calculated) {
        current_block_->Run(this);
        moves_calculated = true;
        return moves_;
      }
      return moves_;
    }

    MoveRevertInformation Apply(const std::vector<ModifierApplication> &move) {
      Block *previous_search_step = current_block_;
      std::vector<ActionRevertInfo> revert_infos;
      revert_infos.reserve(move.size());
      for (const auto &mod_application : move) {
        vertex_id_t previous_vertex = current_pos_;
        const ModifyingApplication *modifier = blocks_.Modifier(mod_application.modifier_index);
        current_pos_ = mod_application.vertex;
        revert_infos.push_back(ActionRevertInfo{modifier, previous_vertex, modifier->Apply(this)});
      }
      current_block_ = blocks_.Switch(move.back().modifier_index);
      blocks_.ResetStacks();
      MakeKeeperClosure(revert_infos);
      moves_calculated = false;
      moves_.clear();
      return {previous_search_step, revert_infos};
    }

    void Revert(const MoveRevertInformation &revert_info) {
      for (uint i = 0; i < revert_info.player_steps_revert_information.size(); i++) {
        const auto &step = revert_info.player_steps_revert_information[
            revert_info.player_steps_revert_information.size() - i - 1];
        step.application->Revert(this, step.revert_name);
        current_pos_ = step.vertex;
      }
      current_block_ = revert_info.previous_search_step;
      moves_calculated = false;
      moves_.clear();
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

    const std::vector<variable_value_t> &variables_values() const {
      return variables_values_;
    }

    void Reset() {
      current_pos_ = 1;
      variables_values_ = std::vector<variable_value_t>(declarations_.variables_resolver().size(), 0);
      current_player_ = declarations_.keeper_id();
      board_ = declarations_.initial_board().initial_content();
      current_block_ = blocks_.initial();
      blocks_.ResetStacks();
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
      while (exists && current_player_ == declarations_.keeper_id()) {
        applied_modifiers_.clear();
        moves_.clear();
        exists = current_block_->RunAndApplyFirst(this, &steps_revert_information, current_pos_);
        blocks_.ResetStacks();
        if (exists) {
          current_block_ = blocks_.Switch(applied_modifiers_.back().modifier_index);
          blocks_.ResetStacks();
          applied_modifiers_.clear();
        }
      }
    }


    const Declarations declarations_;
    vertex_id_t current_pos_;
    std::vector<variable_value_t> variables_values_;
    player_id_t current_player_;
    BoardContent board_;
    BlocksCollection blocks_;
    Block *current_block_;
    std::vector<ModifierApplication> applied_modifiers_;
    std::vector<std::vector<ModifierApplication>> moves_;
    bool moves_calculated;
  };
}


#endif //RBGGAMEMANAGER_GAME_STATE_H
