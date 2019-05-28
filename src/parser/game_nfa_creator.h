//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_GAME_NFA_CREATOR_H
#define RBGGAMEMANAGER_GAME_NFA_CREATOR_H

#include "../actions/action.h"
#include "../game_nfa/game_moves_description.h"
#include "parser_wrapper.h"
#include "parser_actions.h"

// This structure allows the nfa creator to safe its current state information.
struct RecoverInformation {
  std::unique_ptr<fsm::Nfa<const OptimizedAction *>> nfa_result;
  bool register_modifiers;
  bool reuse_final_as_initial;
  fsm::state_id_t last_final;

  RecoverInformation(std::unique_ptr<fsm::Nfa<const OptimizedAction *>> nfa_result,
                     bool register_modifiers,
                     bool reuse_final_as_initial, fsm::state_id_t last_final)
      : nfa_result(std::move(nfa_result)),
        register_modifiers(register_modifiers),
        reuse_final_as_initial(reuse_final_as_initial),
        last_final(last_final) {}
};

class GameNfaCreator : public AstVisitor {
public:
  // This function extracts current nfa. Nfa can be extracted only once after creation.
  std::unique_ptr<fsm::Nfa<const OptimizedAction *>> ExtractNfa() {
    return std::move(nfa_result_);
  }

  // This function extract current move description. Move description can be extracted only once after creation.
  GameMovesDescription ExtractDescription() {
    return GameMovesDescription(ExtractNfa(), std::move(blocks_states_),
                                std::move(actions_), move_pattern_count_);
  }

  friend GameMovesDescription
  CreateMoves(const rbg_parser::game_move &move, const NameResolver &resolver,
              token_id_t piece_id_threshold, const EdgeResolver &edge_resolver, const GraphBoard &board);

  void dispatch(const rbg_parser::sum &) override;
  void dispatch(const rbg_parser::prioritized_sum &) override;
  void dispatch(const rbg_parser::concatenation &) override;
  void dispatch(const rbg_parser::star_move &) override;
  void dispatch(const rbg_parser::shift &) override;
  void dispatch(const rbg_parser::ons &) override;
  void dispatch(const rbg_parser::off &) override;
  void dispatch(const rbg_parser::assignment &) override;
  void dispatch(const rbg_parser::player_switch &) override;
  void dispatch(const rbg_parser::keeper_switch &) override;
  void dispatch(const rbg_parser::arithmetic_comparison &comparison) override;
  void dispatch(const rbg_parser::move_check &check) override;

private:
  void RegisterModifier(fsm::state_id_t initial_id);

  void StartBlock();

  void StopBlock();

  RecoverInformation StartMovePattern() {
    RecoverInformation information(std::move(nfa_result_), register_modifiers_,
                                   reuse_final_as_initial_, last_final_);
    register_modifiers_ = false;
    nfa_result_ = std::unique_ptr<fsm::Nfa<const OptimizedAction *>>(
        new fsm::Nfa<const OptimizedAction *>());
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

  GameNfaCreator(const NameResolver &resolver, token_id_t piece_id_threshold, const EdgeResolver &edge_resolver,
                 const GraphBoard &board)
      : resolver_(resolver),
        edge_resolver_(edge_resolver),
        graph_board_(board),
        nfa_result_(new fsm::Nfa<const OptimizedAction *>()),
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

  const NameResolver &resolver_;
  const EdgeResolver &edge_resolver_;
  const GraphBoard &graph_board_;
  std::unique_ptr<fsm::Nfa<const OptimizedAction *>> nfa_result_;
  std::unordered_map<std::string, const OptimizedAction *> used_actions_;

  std::vector<std::unique_ptr<OptimizedAction> > actions_;

  bool block_started_;
  std::vector<fsm::state_id_t> blocks_states_;
  bool register_modifiers_;

  bool reuse_final_as_initial_;
  fsm::state_id_t last_final_;

  token_id_t piece_id_threshold_;

  unsigned int move_pattern_count_;

};


#endif //RBGGAMEMANAGER_GAME_NFA_CREATOR_H
