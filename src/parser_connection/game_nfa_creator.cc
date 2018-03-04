//
// Created by shrum on 12.01.18.
//

#include "game_nfa_creator.h"

void GameNfaCreator::dispatch(const rbg_parser::conjunction &move) {
  std::vector<std::unique_ptr<Condition> > rec_result;
  for (const auto &item : move.get_content()) {
    item->accept(*this);
    rec_result.push_back(std::move(condition_result_));
  }
  condition_result_ = std::unique_ptr<Condition>(
      new conditions::Conjunction(std::move(rec_result)));
}

void GameNfaCreator::dispatch(const rbg_parser::alternative &move) {
  std::vector<std::unique_ptr<Condition> > rec_result;
  for (const auto &item : move.get_content()) {
    item->accept(*this);
    rec_result.push_back(std::move(condition_result_));
  }
  condition_result_ = std::unique_ptr<Condition>(
      new conditions::Alternative(std::move(rec_result)));
}

void GameNfaCreator::dispatch(const rbg_parser::negatable_condition &move) {
  move.get_content()->accept(*this);
  condition_result_ = std::unique_ptr<Condition>(
      new conditions::Negation(std::move(condition_result_)));
}

void GameNfaCreator::dispatch(const rbg_parser::comparison &move) {
  std::unique_ptr<ArithmeticOperation> left = CreateOperation(
      move.get_left_side());
  std::unique_ptr<ArithmeticOperation> right = CreateOperation(
      move.get_right_side());
  switch (move.get_kind_of_comparison().get_type()) {
    case rbg_parser::not_equal:
      condition_result_ = std::unique_ptr<Condition>(
          new conditions::NotEqual(std::move(left), std::move(right)));
      break;
    case rbg_parser::double_equal:
      condition_result_ = std::unique_ptr<Condition>(
          new conditions::Equal(std::move(left), std::move(right)));
      break;
    case rbg_parser::less:
      condition_result_ = std::unique_ptr<Condition>(
          new conditions::Less(std::move(left), std::move(right)));
      break;
    case rbg_parser::less_equal:
      condition_result_ = std::unique_ptr<Condition>(
          new conditions::LessEqual(std::move(left), std::move(right)));
      break;
    case rbg_parser::greater:
      condition_result_ = std::unique_ptr<Condition>(
          new conditions::Less(std::move(right), std::move(left)));
      break;
    case rbg_parser::greater_equal:
      condition_result_ = std::unique_ptr<Condition>(
          new conditions::LessEqual(std::move(right), std::move(left)));
      break;
    case rbg_parser::left_round_bracket:
      break;
    case rbg_parser::right_round_bracket:
      break;
    case rbg_parser::left_square_bracket:
      break;
    case rbg_parser::right_square_bracket:
      break;
    case rbg_parser::tilde:
      break;
    case rbg_parser::hash:
      break;
    case rbg_parser::minus:
      break;
    case rbg_parser::plus:
      break;
    case rbg_parser::caret:
      break;
    case rbg_parser::star:
      break;
    case rbg_parser::comma:
      break;
    case rbg_parser::semicolon:
      break;
    case rbg_parser::dollar:
      break;
    case rbg_parser::at_sign:
      break;
    case rbg_parser::equal:
      break;
    case rbg_parser::arrow:
      break;
    case rbg_parser::exclamation:
      break;
    case rbg_parser::question:
      break;
    case rbg_parser::logical_and:
      break;
    case rbg_parser::logical_or:
      break;
    case rbg_parser::logical_not:
      break;
    case rbg_parser::player:
      break;
    case rbg_parser::game:
      break;
    case rbg_parser::players:
      break;
    case rbg_parser::pieces:
      break;
    case rbg_parser::variables:
      break;
    case rbg_parser::rules:
      break;
    case rbg_parser::board:
      break;
    case rbg_parser::number:
      break;
    case rbg_parser::identifier:
      break;
    case rbg_parser::quotation:
      break;
    case rbg_parser::dummy:
      break;
    case rbg_parser::finalizer:
      break;
    case rbg_parser::double_plus:
      break;
    case rbg_parser::double_minus:
      break;
    case rbg_parser::bound:
      break;
  }
}

void GameNfaCreator::dispatch(const rbg_parser::move_condition &move) {
  auto result = StartMovePattern();
  move.get_content()->accept(*this);
  condition_result_ = std::unique_ptr<Condition>(
      new conditions::MovePattern(move_pattern_count_, ExtractNfa()));
  move_pattern_count_++;
  StopMovePattern(std::move(result));
}

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
    StopReusingFinals();
  }
  nfa_result_->set_initial(new_initial_id);
  nfa_result_->set_final(new_final_id);
  last_final_ = new_final_id;
  StartReusingFinals();
}

void GameNfaCreator::dispatch(const rbg_parser::pure_sum &move) {
  if (used_actions_.find("epsilon") == used_actions_.end()) {
    std::unique_ptr<Action> empty_action(new actions::Empty());
    used_actions_["epsilon"] = empty_action.get();
    actions_.push_back(std::move(empty_action));
  }

  fsm::state_id_t new_initial_id = NewInitial();
  fsm::state_id_t new_final_id = nfa_result_->NewState();

  for (const std::unique_ptr<rbg_parser::pure_game_move> &child : move.get_content()) {
    child->accept(*this);
    (*nfa_result_)[new_initial_id].AddTransition(nfa_result_->initial(),
                                                 used_actions_["epsilon"]);
    (*nfa_result_)[nfa_result_->final()].AddTransition(new_final_id,
                                                       used_actions_["epsilon"]);
    StopReusingFinals();
  }
  nfa_result_->set_initial(new_initial_id);
  nfa_result_->set_final(new_final_id);
  last_final_ = new_final_id;
  StartReusingFinals();
}

void GameNfaCreator::dispatch(const rbg_parser::concatenation &move) {
  move.get_content().front()->accept(*this);

  fsm::state_id_t initial = nfa_result_->initial();

  for (size_t i = 1; i < move.get_content().size(); i++) {
    const auto &child = move.get_content()[i];
    StartReusingFinals();
    child->accept(*this);
  }
  nfa_result_->set_initial(initial);
  last_final_ = nfa_result_->final();
  StartReusingFinals();
}

void GameNfaCreator::dispatch(const rbg_parser::pure_concatenation &move) {
  move.get_content().front()->accept(*this);

  fsm::state_id_t initial = nfa_result_->initial();

  for (size_t i = 1; i < move.get_content().size(); i++) {
    const auto &child = move.get_content()[i];
    StartReusingFinals();
    child->accept(*this);
  }
  nfa_result_->set_initial(initial);
  last_final_ = nfa_result_->final();
  StartReusingFinals();
}

void GameNfaCreator::dispatch(const rbg_parser::bracketed_move &move) {
  if (move.is_star()) {
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
    StartReusingFinals();
    return;
  }

  move.get_content()->accept(*this);

  fsm::state_id_t initial = nfa_result_->initial();

  for (size_t i = 1; i < move.get_number_of_repetitions(); i++) {
    StartReusingFinals();
    move.get_content()->accept(*this);
  }
  nfa_result_->set_initial(initial);
  last_final_ = nfa_result_->final();
  StartReusingFinals();
}

void GameNfaCreator::dispatch(const rbg_parser::pure_bracketed_move &move) {
  if (move.is_star()) {
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
    StartReusingFinals();
    return;
  }

  move.get_content()->accept(*this);

  fsm::state_id_t initial = nfa_result_->initial();

  for (size_t i = 1; i < move.get_number_of_repetitions(); i++) {
    StartReusingFinals();
    move.get_content()->accept(*this);
  }
  nfa_result_->set_initial(initial);
  last_final_ = nfa_result_->final();
  StartReusingFinals();
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
  StartReusingFinals();
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
  StartReusingFinals();
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
  if (move.is_lazy()) {
    std::unique_ptr<Action> lazy_letter(new actions::Lazy(letter_ptr,
                                                          (unsigned int) blocks_states_.size() -
                                                          1));
    letter_ptr = lazy_letter.get();
    actions_.push_back(std::move(lazy_letter));
  }
  (*nfa_result_)[initial_id].AddTransition(final_id, letter_ptr);
  nfa_result_->set_initial(initial_id);
  nfa_result_->set_final(final_id);

  last_final_ = final_id;
  StartReusingFinals();
}

void GameNfaCreator::dispatch(const rbg_parser::assignment &move) {
  fsm::state_id_t initial_id = NewInitial();
  fsm::state_id_t final_id = nfa_result_->NewState();
  RegisterModifier(initial_id);
  token_id_t variable_id = resolver_.Id(move.get_left_side().to_string());
  std::unique_ptr<Action> letter;
  if (move.is_incrementation()) {
    letter = std::unique_ptr<Action>(new actions::Incrementation(variable_id,
                                                                 (unsigned int) blocks_states_.size() -
                                                                 1));
  } else if (move.is_decrementation()) {
    letter = std::unique_ptr<Action>(new actions::Decrementation(variable_id,
                                                                 (unsigned int) blocks_states_.size() -
                                                                 1));
  } else {
    letter = std::unique_ptr<Action>(new actions::Assignment(
        variable_id, CreateOperation(move.get_right_side()),
        (unsigned int) blocks_states_.size() - 1));
  }
  Action *letter_ptr = letter.get();
  actions_.push_back(std::move(letter));
  if (move.is_lazy()) {
    std::unique_ptr<Action> lazy_letter(new actions::Lazy(letter_ptr,
                                                          (unsigned int) blocks_states_.size() -
                                                          1));
    letter_ptr = lazy_letter.get();
    actions_.push_back(std::move(lazy_letter));
  }
  (*nfa_result_)[initial_id].AddTransition(final_id, letter_ptr);
  nfa_result_->set_initial(initial_id);
  nfa_result_->set_final(final_id);

  last_final_ = final_id;
  StartReusingFinals();
}

void GameNfaCreator::dispatch(const rbg_parser::player_switch &move) {
  fsm::state_id_t initial_id = NewInitial();
  fsm::state_id_t final_id = nfa_result_->NewState();
  RegisterModifier(initial_id);
  std::unique_ptr<Action> letter;
  if (move.is_deterministic_keeper()) {
    letter = std::unique_ptr<Action>(
        new actions::PlayerSwitch(resolver_.Id("_epsilon"),
                                  (unsigned int) blocks_states_.size() - 1));
  } else if (move.is_non_deterministic_keeper()) {
    letter = std::unique_ptr<Action>(
        new actions::PlayerSwitch(resolver_.Id("_*"),
                                  (unsigned int) blocks_states_.size() - 1));
  } else {
    token_id_t player_id = resolver_.Id(move.get_player().to_string());
    letter = std::unique_ptr<Action>(new actions::PlayerSwitch(player_id,
                                                               (unsigned int) blocks_states_.size() -
                                                               1));
  }
  Action *letter_ptr = letter.get();
  actions_.push_back(std::move(letter));
  (*nfa_result_)[initial_id].AddTransition(final_id, letter_ptr);
  nfa_result_->set_initial(initial_id);
  nfa_result_->set_final(final_id);

  last_final_ = final_id;
  StartReusingFinals();
}

void GameNfaCreator::RegisterModifier(fsm::state_id_t initial_id) {
  if (!block_started_ && register_modifiers_)
    blocks_states_.push_back(initial_id);
}

void GameNfaCreator::dispatch(const rbg_parser::condition_check &move) {
  std::string move_identifier = move.to_rbg();
  if (used_actions_.find(move_identifier) == used_actions_.end()) {
    move.get_content()->accept(*this);
    std::unique_ptr<Action> action(
        new actions::ConditionAction(ExtractCondition()));
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
  StartReusingFinals();
}

void GameNfaCreator::dispatch(const rbg_parser::modifier_block &move) {

  move.get_content().front()->accept(*this);

  fsm::state_id_t initial = nfa_result_->initial();
  StartBlock();
  for (size_t i = 1; i < move.get_content().size(); i++) {
    const auto &child = move.get_content()[i];
    StartReusingFinals();
    child->accept(*this);
  }
  StopBlock();
  nfa_result_->set_initial(initial);
  last_final_ = nfa_result_->final();
  StartReusingFinals();
}

void GameNfaCreator::StopBlock() { block_started_ = false; }

void GameNfaCreator::StartBlock() { block_started_ = true; }