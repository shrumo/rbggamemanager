//
// Created by shrum on 12.01.18.
//

#ifndef RBGGAMEMANAGER_GAME_STATE_H
#define RBGGAMEMANAGER_GAME_STATE_H


#include <utility>

#include "game_description.h"
#include "lazy_evaluator.h"
#include "search_context.h"

// This MoveResult can be used to revert moves and get
// information about move.
class MoveResult {
public:
  MoveResult(std::vector<ActionResult> actions_results, size_t previous_x,
             size_t previous_y,
             fsm::state_id_t previous_state,
             std::vector<ActionApplication> applied_modifiers)
      : actions_results_(std::move(actions_results)),
        previous_x_(previous_x),
        previous_y_(previous_y),
        previous_state_(previous_state),
        applied_modifiers_(std::move(applied_modifiers)) {}

  const std::vector<ActionResult> &actions_results() const {
    return actions_results_;
  }

  const std::vector<ActionApplication> &applied_modifiers() const {
    return applied_modifiers_;
  }

  size_t previous_x() const {
    return previous_x_;
  }

  size_t previous_y() const {
    return previous_y_;
  }

  fsm::state_id_t previous_state() const {
    return previous_state_;
  }

private:
  std::vector<ActionResult> actions_results_;
  size_t previous_x_, previous_y_;
  fsm::state_id_t previous_state_;
  std::vector<ActionApplication> applied_modifiers_;
};

class GameState {
public:
  // Lazy controller is separated from main class
  friend class LazyEvaluator;

  // Modifying actions

  friend class actions::Shift;

  friend class actions::Off;

  friend class actions::PlayerSwitch;

  friend class actions::Assignment;

  friend class actions::Lazy;

  friend class actions::Incrementation;

  friend class actions::Decrementation;

  // Move pattern will be able to use the state_ to check the condition
  // This will speed up things. (We do not have to make a copy.)
  friend class conditions::MovePattern;

  explicit GameState(const GameDescription &description)
      : parent_(description),
        current_board_(description.initial_board()),
        current_x_(0), current_y_(0),
        current_state_(description.moves_description().nfa().initial()),
        sigma_(description.VariablesCount(), 0),
        current_player_(description.resolver().Id("_epsilon")) {
    for (size_t y = 0; y < current_board_.height(); y++) {
      for (size_t x = 0; x < current_board_.width(); x++) {
        sigma_[current_board_(x, y)]++;
      }
    }
  }

  // Resets the game to the initial state_.
  void Reset() {
    current_board_ = parent_.initial_board();
    current_x_ = 0;
    current_y_ = 0;
    current_state_ = parent_.moves_description().nfa().initial();
    std::fill(sigma_.begin(), sigma_.end(), 0);
    for (size_t y = 0; y < current_board_.height(); y++) {
      for (size_t x = 0; x < current_board_.width(); x++) {
        sigma_[current_board_(x, y)]++;
      }
    }
    current_player_ = parent_.resolver().Id("_epsilon");
  }

  fsm::state_id_t nfa_state() const {
    return current_state_;
  }

  // Returns the piece at current position.
  token_id_t CurrentPiece() const {
    return current_board_(current_x_, current_y_);
  }

  const GameDescription &description() const {
    return parent_;
  }

  int Value(token_id_t variable) const {
    return sigma_[variable];
  }

  const Board &board() const {
    return current_board_;
  }

  size_t x() const {
    return current_x_;
  }

  size_t y() const {
    return current_y_;
  }

  size_t width() const {
    return current_board_.width();
  }

  size_t height() const {
    return current_board_.height();
  }

  token_id_t player() const {
    return current_player_;
  }

  void MakeMove(const Move &move) {
    for (const auto &block : move.blocks()) {
      current_state_ = parent_.moves_description().CorrespondingState(
          block.id());
      current_x_ = block.x();
      current_y_ = block.y();
      while (
          parent_.moves_description().nfa()[current_state_].transitions().size() ==
          1 &&
          parent_.moves_description().nfa()[current_state_].transitions().front().letter()->index() ==
          block.id()) {
        parent_.moves_description().nfa()[current_state_].transitions().front().letter()->Apply(
            this);
        current_state_ = parent_.moves_description().nfa()[current_state_].transitions().front().target();
      }
    }
    lazy_controller_.Clear();
  }

  MoveResult MakeRevertibleMove(const Move &move) {
    std::vector<ActionResult> results;
    std::vector<ActionApplication> applied_modifiers;
    size_t previous_x = current_x_;
    size_t previous_y = current_y_;
    fsm::state_id_t previous_state = current_state_;
    for (const auto &block : move.blocks()) {
      current_state_ = parent_.moves_description().CorrespondingState(
          block.id());
      current_x_ = block.x();
      current_y_ = block.y();
      while (
          parent_.moves_description().nfa()[current_state_].transitions().size() ==
          1 &&
          parent_.moves_description().nfa()[current_state_].transitions().front().letter()->index() ==
          block.id()) {
        results.push_back(
            parent_.moves_description().nfa()[current_state_].transitions().front().letter()->Apply(
                this));
        applied_modifiers.emplace_back(current_x_, current_y_,
                                       parent_.moves_description().nfa()[current_state_].transitions().front().letter());
        current_state_ = parent_.moves_description().nfa()[current_state_].transitions().front().target();
      }
    }
    return {std::move(results), previous_x, previous_y, previous_state,
            std::move(applied_modifiers)};
  }

  void RevertMove(const MoveResult &information) {
    for (ssize_t i = information.applied_modifiers().size() - 1; i >= 0; i--) {
      const auto &action = information.applied_modifiers()[i];
      RevertActionApplication(action, information.actions_results()[i]);
    }
    current_x_ = information.previous_x();
    current_y_ = information.previous_y();
    current_state_ = information.previous_state();
  }

  std::vector<Move> FindMoves(SearchContext *context, ssize_t max_depth = -1) {
    current_search_ = context;
    auto result = current_search_->FindMoves(this, max_depth);
    current_search_ = nullptr;
    return std::move(result);
  }

  std::vector<Move>
  FindFirstMove(SearchContext *context, ssize_t max_depth = -1) {
    current_search_ = context;
    auto result = current_search_->FindFirstMove(this, max_depth);
    current_search_ = nullptr;
    return std::move(result);
  }

private:
  const GameDescription &parent_;

  LazyEvaluator lazy_controller_;

  Board current_board_;
  size_t current_x_, current_y_;
  fsm::state_id_t current_state_;

  std::vector<int> sigma_;
  token_id_t current_player_;

  SearchContext *current_search_;

  void SetPiece(token_id_t piece) {
    sigma_[CurrentPiece()]--;
    current_board_(current_x_, current_y_) = piece;
    sigma_[piece]++;
  }

  LazyEvaluator &lazy() {
    return lazy_controller_;
  }

  bool SearchingForMoves() {
    return current_search_ != nullptr;
  }

  SearchContext &search_context() {
    return *current_search_;
  }

  ActionResult ApplyActionApplication(const ActionApplication &application);

  void RevertActionApplication(const ActionApplication &application,
                               const ActionResult &application_result);
};

std::ostream &operator<<(std::ostream &s, const GameState &state);

#endif //RBGGAMEMANAGER_GAME_STATE_H
