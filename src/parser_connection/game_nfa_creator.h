//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_GAME_NFA_CREATOR_H
#define RBGGAMEMANAGER_GAME_NFA_CREATOR_H

#include "../../rbgParser/src/sum.hpp"
#include "../../rbgParser/src/concatenation.hpp"
#include "../../rbgParser/src/shift.hpp"
#include "../../rbgParser/src/ons.hpp"
#include "../../rbgParser/src/offs.hpp"
#include "../../rbgParser/src/assignments.hpp"
#include "../../rbgParser/src/switch.hpp"
#include "../../rbgParser/src/condition_check.hpp"
#include "../../rbgParser/src/power_move.hpp"
#include "../../rbgParser/src/star_move.hpp"
#include "../../rbgParser/src/modifier_block.hpp"
#include "../actions/action.h"
#include "../game_nfa/game_moves_description.h"

// This structure alows the nfa creator to safe its current state information.
struct RecoverInformation {
  std::unique_ptr<fsm::Nfa<const Action *>> nfa_result;
  bool register_modifiers;
  bool reuse_final_as_initial;
  fsm::state_id_t last_final;

  RecoverInformation(std::unique_ptr<fsm::Nfa<const Action *>> nfa_result,
                     bool register_modifiers,
                     bool reuse_final_as_initial, fsm::state_id_t last_final)
      : nfa_result(std::move(nfa_result)),
        register_modifiers(register_modifiers),
        reuse_final_as_initial(reuse_final_as_initial),
        last_final(last_final) {}
};

class GameNfaCreator : public rbg_parser::abstract_dispatcher {
public:
  // This function extracts current nfa. Nfa can be extracted only once after creation.
  std::unique_ptr<fsm::Nfa<const Action *>> ExtractNfa() {
    return std::move(nfa_result_);
  }

  // This function extract current move description. Move description can be extracted only once after creation.
  GameMovesDescription ExtractDescription() {
    return GameMovesDescription(ExtractNfa(), std::move(blocks_states_),
                                std::move(actions_), move_pattern_count_);
  }

  friend GameMovesDescription
  create_moves(const rbg_parser::game_move &move, const NameResolver &resolver,
               token_id_t piece_id_threshold);


  void dispatch(const rbg_parser::sum &) override;

  void dispatch(const rbg_parser::concatenation &) override;

  void dispatch(const rbg_parser::conditional_sum&) override;

  void dispatch(const rbg_parser::power_move&) override;

  void dispatch(const rbg_parser::star_move&) override;

  void dispatch(const rbg_parser::conditional_star_move&) override;

  void dispatch(const rbg_parser::shift &) override;

  void dispatch(const rbg_parser::ons &) override;

  void dispatch(const rbg_parser::off &) override;

  void dispatch(const rbg_parser::assignment &) override;

  void dispatch(const rbg_parser::player_switch &) override;

  void dispatch(const rbg_parser::keeper_switch&) override;

  void dispatch(const rbg_parser::condition_check &) override;

  void dispatch(const rbg_parser::modifier_block &) override;

  void dispatch(const rbg_parser::arithmetic_comparison &comparison) override;

  void dispatch(const rbg_parser::player_check &check) override;

  void dispatch(const rbg_parser::integer_arithmetic &arithmetic) override {}

  void dispatch(const rbg_parser::variable_arithmetic &arithmetic) override {}

  void dispatch(const rbg_parser::multiply_arithmetic &arithmetic) override {}

  void dispatch(const rbg_parser::sum_arithmetic &arithmetic) override {}

private:
  void RegisterModifier(fsm::state_id_t initial_id);

  void StartBlock();

  void StopBlock();

  RecoverInformation StartMovePattern() {
    RecoverInformation information(std::move(nfa_result_), register_modifiers_,
                                   reuse_final_as_initial_, last_final_);
    register_modifiers_ = false;
    nfa_result_ = std::unique_ptr<fsm::Nfa<const Action *>>(
        new fsm::Nfa<const Action *>());
    last_final_ = 0;
    reuse_final_as_initial_ = false;
    return information;
  }

  void StopMovePattern(RecoverInformation information) {
    nfa_result_ = std::move(information.nfa_result);
    register_modifiers_ = information.register_modifiers;
    reuse_final_as_initial_ = information.reuse_final_as_initial;
    last_final_ = information.last_final;
  }

  GameNfaCreator(const NameResolver &resolver, token_id_t piece_id_threshold)
      : resolver_(resolver),
        nfa_result_(new fsm::Nfa<const Action *>()),
        block_started_(false),
        register_modifiers_(true),
        reuse_final_as_initial_(false),
        last_final_(0),
        piece_id_threshold_(piece_id_threshold),
        move_pattern_count_(0) {
    blocks_states_.push_back(0);
  }

  void ReuseFinal() {
    reuse_final_as_initial_ = true;
  }

  fsm::state_id_t NewInitial() {
    if (reuse_final_as_initial_) {
      reuse_final_as_initial_ = false;
      return last_final_;
    }
    return nfa_result_->NewState();
  }

//  std::unique_ptr<ArithmeticOperation>
//  CreateOperation(const rbg_parser::token &token) {
//    std::unique_ptr<ArithmeticOperation> result;
//    if (token.get_type() == rbg_parser::number) {
//      result = std::unique_ptr<ArithmeticOperation>(
//          new arithmetic_operations::Constant(token.get_value()));
//    } else {
//      token_id_t variable_id = resolver_.Id(token.to_string());
//      result = std::unique_ptr<ArithmeticOperation>(
//          new arithmetic_operations::Variable(variable_id));
//    }
//    return result;
//  }


  const NameResolver &resolver_;
  std::unique_ptr<fsm::Nfa<const Action *>> nfa_result_;
  std::unordered_map<std::string, const Action *> used_actions_;

  std::vector<std::unique_ptr<Action> > actions_;

  bool block_started_;
  std::vector<fsm::state_id_t> blocks_states_;
  bool register_modifiers_;

  bool reuse_final_as_initial_;
  fsm::state_id_t last_final_;

  token_id_t piece_id_threshold_;

  unsigned int move_pattern_count_;

};


#endif //RBGGAMEMANAGER_GAME_NFA_CREATOR_H
