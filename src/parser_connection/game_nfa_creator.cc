//
// Created by shrum on 12.01.18.
//

#include "game_nfa_creator.h"
#include "arithmetic_creator.h"

void GameNfaCreator::dispatch(const rbg_parser::sum &move) {
  if (used_actions_.find("epsilon") == used_actions_.end()) {
    std::unique_ptr<Action> empty_action(new actions::Empty());
    used_actions_["epsilon"] = empty_action.get();
    actions_.push_back(std::move(empty_action));
  }

  fsm::state_id_t new_initial_id = NewInitial();
  fsm::state_id_t new_final_id = nfa_result_->NewState();

  for (const std::unique_ptr<rbg_parser::game_move> &child : move.get_content()) {
    child->accept(*this);
    (*nfa_result_)[new_initial_id].AddTransition(nfa_result_->initial(),
                                                 used_actions_["epsilon"]);
    (*nfa_result_)[nfa_result_->final()].AddTransition(new_final_id,
                                                       used_actions_["epsilon"]);
  }
  nfa_result_->set_initial(new_initial_id);
  nfa_result_->set_final(new_final_id);
  last_final_ = new_final_id;
}


void GameNfaCreator::dispatch(const rbg_parser::concatenation &move) {
  move.get_content().front()->accept(*this);

  fsm::state_id_t initial = nfa_result_->initial();

  for (size_t i = 1; i < move.get_content().size(); i++) {
    const auto &child = move.get_content()[i];
    ReuseFinal();
    child->accept(*this);
  }
  nfa_result_->set_initial(initial);
  last_final_ = nfa_result_->final();
}

void GameNfaCreator::dispatch(const rbg_parser::shift &move) {
  std::string move_identifier = move.to_rbg();
  if (used_actions_.find(move_identifier) == used_actions_.end()) {
    std::unique_ptr<Action> action(
        new actions::Shift(move.get_x(), move.get_y()));
    used_actions_[move_identifier] = action.get();
    actions_.push_back(std::move(action));
  }
  fsm::state_id_t initial_id = NewInitial();
  fsm::state_id_t final_id = nfa_result_->NewState();
  (*nfa_result_)[initial_id].AddTransition(final_id,
                                           used_actions_[move_identifier]);
  nfa_result_->set_initial(initial_id);
  nfa_result_->set_final(final_id);

  last_final_ = final_id;
}

void GameNfaCreator::dispatch(const rbg_parser::ons &move) {
  std::string move_identifier = move.to_rbg();
  if (used_actions_.find(move_identifier) == used_actions_.end()) {
    std::vector<bool> pieces(piece_id_threshold_, false);
    for (const auto &piece_token : move.get_legal_ons()) {
      token_id_t piece_id = resolver_.Id(piece_token.to_string());
      pieces[piece_id] = true;
    }
    std::unique_ptr<Action> action(new actions::On(std::move(pieces)));
    used_actions_[move_identifier] = action.get();
    actions_.push_back(std::move(action));
  }
  fsm::state_id_t initial_id = NewInitial();
  fsm::state_id_t final_id = nfa_result_->NewState();
  (*nfa_result_)[initial_id].AddTransition(final_id,
                                           used_actions_[move_identifier]);
  nfa_result_->set_initial(initial_id);
  nfa_result_->set_final(final_id);

  last_final_ = final_id;
}

void GameNfaCreator::dispatch(const rbg_parser::off &move) {
  fsm::state_id_t initial_id = NewInitial();
  fsm::state_id_t final_id = nfa_result_->NewState();
  RegisterModifier(initial_id);
  token_id_t piece_id = resolver_.Id(move.get_piece().to_string());
  std::unique_ptr<Action> letter(
      new actions::Off(piece_id, (unsigned int) blocks_states_.size() - 1));
  Action *letter_ptr = letter.get();
  actions_.push_back(std::move(letter));
  (*nfa_result_)[initial_id].AddTransition(final_id, letter_ptr);
  nfa_result_->set_initial(initial_id);
  nfa_result_->set_final(final_id);

  last_final_ = final_id;
}

void GameNfaCreator::RegisterModifier(fsm::state_id_t initial_id) {
  if (!block_started_ && register_modifiers_)
    blocks_states_.push_back(initial_id);
}

void GameNfaCreator::dispatch(const rbg_parser::condition_check &move) {
  std::string move_identifier = move.to_rbg();
  if (used_actions_.find(move_identifier) == used_actions_.end()) {

    auto r = StartMovePattern();
    move.get_content()->accept(*this);
    std::unique_ptr<Action> action;
    if(!move.is_negated()) // TODO(shrum): Ask why I need to negate the thing
    {
      action = std::unique_ptr<Action>(new actions::NegatedConditionCheck(move_pattern_count_++,ExtractNfa()));
    }
    else
    {
      action = std::unique_ptr<Action>(new actions::ConditionCheck(move_pattern_count_++,ExtractNfa()));
    }
    used_actions_[move_identifier] = action.get();
    actions_.push_back(std::move(action));
    StopMovePattern(std::move(r));
  }
  fsm::state_id_t initial_id = NewInitial();
  fsm::state_id_t final_id = nfa_result_->NewState();
  (*nfa_result_)[initial_id].AddTransition(final_id,
                                           used_actions_[move_identifier]);
  nfa_result_->set_initial(initial_id);
  nfa_result_->set_final(final_id);

  last_final_ = final_id;
}

void GameNfaCreator::dispatch(const rbg_parser::modifier_block &move) {

  move.get_content().front()->accept(*this);

  fsm::state_id_t initial = nfa_result_->initial();
  StartBlock();
  for (size_t i = 1; i < move.get_content().size(); i++) {
    const auto &child = move.get_content()[i];
    ReuseFinal();
    child->accept(*this);
  }
  StopBlock();
  nfa_result_->set_initial(initial);
  last_final_ = nfa_result_->final();
}

void GameNfaCreator::StopBlock() { block_started_ = false; }

void GameNfaCreator::StartBlock() { block_started_ = true; }

void GameNfaCreator::dispatch(const rbg_parser::conditional_sum &) {
  // TODO(shrum): Introduce sth new here.
  exit(0);
}

void GameNfaCreator::dispatch(const rbg_parser::assignment &move) {
  fsm::state_id_t initial_id = NewInitial();
  fsm::state_id_t final_id = nfa_result_->NewState();
  RegisterModifier(initial_id);
  token_id_t variable_id = resolver_.Id(move.get_left_side().to_string());
  std::unique_ptr<Action> letter;
  letter = std::unique_ptr<Action>(
      new actions::Assignment(variable_id, CreateOperation(*move.get_right_side(),resolver_),
                              (unsigned int) blocks_states_.size() - 1));

  Action *letter_ptr = letter.get();
  actions_.push_back(std::move(letter));
  (*nfa_result_)[initial_id].AddTransition(final_id, letter_ptr);
  nfa_result_->set_initial(initial_id);
  nfa_result_->set_final(final_id);
  last_final_ = final_id;
}

void GameNfaCreator::dispatch(const rbg_parser::player_switch &move) {
  fsm::state_id_t initial_id = NewInitial();
  fsm::state_id_t final_id = nfa_result_->NewState();
  RegisterModifier(initial_id);
  std::unique_ptr<Action> letter;
  token_id_t player_id = resolver_.Id(move.get_player().to_string());
  letter = std::unique_ptr<Action>(new actions::PlayerSwitch(player_id,
                                                               (unsigned int) blocks_states_.size() -
                                                               1));
  Action *letter_ptr = letter.get();
  actions_.push_back(std::move(letter));
  (*nfa_result_)[initial_id].AddTransition(final_id, letter_ptr);
  nfa_result_->set_initial(initial_id);
  nfa_result_->set_final(final_id);

  last_final_ = final_id;
}

void GameNfaCreator::dispatch(const rbg_parser::power_move &move) {
  move.get_content()->accept(*this);

  fsm::state_id_t initial = nfa_result_->initial();

  for (size_t i = 1; i < move.get_number_of_repetitions(); i++) {
    ReuseFinal();
    move.get_content()->accept(*this);
  }
  nfa_result_->set_initial(initial);
  last_final_ = nfa_result_->final();
}

void GameNfaCreator::dispatch(const rbg_parser::conditional_star_move &) {
  // TODO(shrum): Introduce sth new here.
  exit(0);
}

void GameNfaCreator::dispatch(const rbg_parser::keeper_switch &) {
  fsm::state_id_t initial_id = NewInitial();
  fsm::state_id_t final_id = nfa_result_->NewState();
  RegisterModifier(initial_id);
  std::unique_ptr<Action> letter;
  token_id_t player_id = resolver_.Id("_*");
  letter = std::unique_ptr<Action>(new actions::PlayerSwitch(player_id,
                                                             (unsigned int) blocks_states_.size() -
                                                             1));
  Action *letter_ptr = letter.get();
  actions_.push_back(std::move(letter));
  (*nfa_result_)[initial_id].AddTransition(final_id, letter_ptr);
  nfa_result_->set_initial(initial_id);
  nfa_result_->set_final(final_id);

  last_final_ = final_id;
}

void GameNfaCreator::dispatch(const rbg_parser::star_move &move) {
  if (used_actions_.find("epsilon") == used_actions_.end()) {
      std::unique_ptr<Action> empty_action(new actions::Empty());
      used_actions_["epsilon"] = empty_action.get();
      actions_.push_back(std::move(empty_action));
    }

  fsm::state_id_t new_initial_id = NewInitial();
  fsm::state_id_t new_final_id = nfa_result_->NewState();

  move.get_content()->accept(*this);

  (*nfa_result_)[new_initial_id].AddTransition(new_final_id,
                                               used_actions_["epsilon"]);
  (*nfa_result_)[new_initial_id].AddTransition(nfa_result_->initial(),
                                               used_actions_["epsilon"]);
  (*nfa_result_)[nfa_result_->final()].AddTransition(new_final_id,
                                                     used_actions_["epsilon"]);
  (*nfa_result_)[nfa_result_->final()].AddTransition(nfa_result_->initial(),
                                                     used_actions_["epsilon"]);

  nfa_result_->set_initial(new_initial_id);
  nfa_result_->set_final(new_final_id);
  last_final_ = new_final_id;
}

void GameNfaCreator::dispatch(const rbg_parser::arithmetic_comparison &comparison) {
  std::string move_identifier = comparison.to_rbg();

  if (used_actions_.find(move_identifier) == used_actions_.end()) {
    switch (comparison.get_kind_of_comparison()) {
      case rbg_parser::eq: {
        std::unique_ptr<Action> action(
            new actions::ArithmeticEqualComparison(CreateOperation(*comparison.get_left_side(), resolver_),
                                                   CreateOperation(*comparison.get_right_side(), resolver_)));
        used_actions_[move_identifier] = action.get();
        actions_.push_back(std::move(action));
        break;
      }
      case rbg_parser::neq: {
        std::unique_ptr<Action> action(
            new actions::ArithmeticNotEqualComparison(CreateOperation(*comparison.get_left_side(), resolver_),
                                                   CreateOperation(*comparison.get_right_side(), resolver_)));
        used_actions_[move_identifier] = action.get();
        actions_.push_back(std::move(action));
        break;
      }
      case rbg_parser::le: {
        std::unique_ptr<Action> action(
            new actions::ArithmeticLessComparison(CreateOperation(*comparison.get_left_side(), resolver_),
                                                   CreateOperation(*comparison.get_right_side(), resolver_)));
        used_actions_[move_identifier] = action.get();
        actions_.push_back(std::move(action));
        break;
      }
      case rbg_parser::leq: {
        std::unique_ptr<Action> action(
            new actions::ArithmeticLessEqualComparison(CreateOperation(*comparison.get_left_side(), resolver_),
                                                   CreateOperation(*comparison.get_right_side(), resolver_)));
        used_actions_[move_identifier] = action.get();
        actions_.push_back(std::move(action));
        break;
      }
      case rbg_parser::ge: {
        std::unique_ptr<Action> action(
            new actions::ArithmeticLessComparison(CreateOperation(*comparison.get_right_side(), resolver_),
                                                   CreateOperation(*comparison.get_left_side(), resolver_)));
        used_actions_[move_identifier] = action.get();
        actions_.push_back(std::move(action));
        break;
      }
      case rbg_parser::geq: {
        std::unique_ptr<Action> action(
            new actions::ArithmeticLessEqualComparison(CreateOperation(*comparison.get_right_side(), resolver_),
                                                   CreateOperation(*comparison.get_left_side(), resolver_)));
        used_actions_[move_identifier] = action.get();
        actions_.push_back(std::move(action));
        break;
      }
    }
  }
  fsm::state_id_t initial_id = NewInitial();
  fsm::state_id_t final_id = nfa_result_->NewState();
  (*nfa_result_)[initial_id].AddTransition(final_id,
                                           used_actions_[move_identifier]);
  nfa_result_->set_initial(initial_id);
  nfa_result_->set_final(final_id);

  last_final_ = final_id;
}

void GameNfaCreator::dispatch(const rbg_parser::player_check &move) {
  std::string move_identifier = move.to_rbg();
  if (used_actions_.find(move_identifier) == used_actions_.end()) {
    token_id_t player = resolver_.Id(move.get_player_to_check().to_string());
    std::unique_ptr<Action> action(new actions::PlayerCheck(player));
    used_actions_[move_identifier] = action.get();
    actions_.push_back(std::move(action));
  }
  fsm::state_id_t initial_id = NewInitial();
  fsm::state_id_t final_id = nfa_result_->NewState();
  (*nfa_result_)[initial_id].AddTransition(final_id,
                                           used_actions_[move_identifier]);
  nfa_result_->set_initial(initial_id);
  nfa_result_->set_final(final_id);

  last_final_ = final_id;
}
