//
// Created by shrum on 16.06.19.
//

#ifndef RBGGAMEMANAGER_MOVES_H
#define RBGGAMEMANAGER_MOVES_H

#include <game_description/board.h>
#include <game_description/moves/arithmetic_operation.h>
#include <utility/graph.h>
#include <utility/move_type.h>

namespace rbg {
class Shift;

class ShiftTable;

class ArithmeticComparison;

class Off;

class On;

class PlayerSwitch;

class KeeperSwitch;

class Assignment;

class PlayerCheck;

class Condition;

class VisitedQuery;

class EmptyBackward;

class EmptyForward;

class Noop;

class MoveVisitor {
public:
  virtual void Visit(const Shift &) = 0;

  virtual void Visit(const ShiftTable &) = 0;

  virtual void Visit(const ArithmeticComparison &) = 0;

  virtual void Visit(const Off &) = 0;

  virtual void Visit(const On &) = 0;

  virtual void Visit(const PlayerSwitch &) = 0;

  virtual void Visit(const KeeperSwitch &) = 0;

  virtual void Visit(const Assignment &) = 0;

  virtual void Visit(const PlayerCheck &) = 0;

  virtual void Visit(const Condition &) = 0;

  virtual void Visit(const VisitedQuery &) = 0;

  virtual void Visit(const EmptyBackward &) = 0;

  virtual void Visit(const EmptyForward &) = 0;

  virtual void Visit(const Noop &) = 0;
};

class Move {
public:
  explicit Move(MoveType type) : type_(type), original_move_(nullptr) {}

  MoveType type() const { return type_; }

  virtual void Accept(MoveVisitor &visitor) const = 0;

  virtual bool indexed() const { return false; }

  virtual ~Move() = default;

  void set_original_move(const rbg_parser::game_move *move) {
    original_move_ = move;
  }

  const rbg_parser::game_move *original_move() const { return original_move_; }

private:
  MoveType type_;
  const rbg_parser::game_move *original_move_;
};

class IndexedMove : public Move {
public:
  explicit IndexedMove(MoveType type, uint index) : Move(type), index_(index) {}

  uint index() const { return index_; }

  bool indexed() const override { return true; }

private:
  uint index_;
};

// Shift changes the current position on board.
class Shift : public IndexedMove {
public:
  explicit Shift(edge_id_t edge_id, uint index)
      : IndexedMove(MoveType::kShiftType, index), edge_id_(edge_id) {}

  // Returns the edge this shift travels.
  edge_id_t edge_id() const { return edge_id_; }

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

private:
  edge_id_t edge_id_;
};

// Shift changes the current position on board.
class ShiftTable : public IndexedMove {
public:
  explicit ShiftTable(std::vector<std::vector<vertex_id_t>> table, uint index)
      : IndexedMove(MoveType::kShiftTableType, index),
        table_(std::move(table)) {
    for (auto &row : table_) {
      std::vector<vertex_id_t> new_row;
      new_row.reserve(row.size());
      for (long element : row) {
        if (element != -1)
          new_row.push_back(element);
      }
      row = std::move(new_row);
      row.shrink_to_fit();
    }
  }

  // Returns the edge this shift travels.
  const std::vector<std::vector<vertex_id_t>> &table() const { return table_; }

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

private:
  std::vector<std::vector<vertex_id_t>> table_;
};

// On checks if one of the pieces is on the current position on board.
class On : public IndexedMove {
public:
  // Parameter pieces should contain true for token ids of pieces names that the
  // on accepts.
  explicit On(std::vector<bool> pieces, uint index)
      : IndexedMove(MoveType::kOnType, index), pieces_(std::move(pieces)) {}

  // Returns the pieces on requires on the board in order to be legal.
  const std::vector<bool> &pieces_table() const { return pieces_; }

  std::vector<piece_id_t> pieces() const {
    std::vector<piece_id_t> result;
    for (piece_id_t i = 0; i < pieces_.size(); i++) {
      if (pieces_[i])
        result.push_back(i);
    }
    return result;
  }

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

private:
  std::vector<bool> pieces_;
};

enum class ComparisonType {
  kEqual,
  kNotEqual,
  kLess,
  kLessEqual,
  kGreater,
  kGreaterEqual
};

// Check arithmetic comparison
class ArithmeticComparison : public IndexedMove {
public:
  ArithmeticComparison(std::unique_ptr<ArithmeticOperation> left,
                       std::unique_ptr<ArithmeticOperation> right,
                       ComparisonType type, uint index)
      : IndexedMove(MoveType::kArithmeticComparisonType, index),
        left_(std::move(left)), right_(std::move(right)),
        comparison_type_(type) {}

  const ArithmeticOperation &left() const { return *left_; }

  const ArithmeticOperation &right() const { return *right_; }

  std::unique_ptr<ArithmeticOperation> ExtractLeft() {
    return std::move(left_);
  }

  std::unique_ptr<ArithmeticOperation> ExtractRight() {
    return std::move(right_);
  }

  ComparisonType comparison_type() const { return comparison_type_; }

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

private:
  std::unique_ptr<ArithmeticOperation> left_, right_;
  ComparisonType comparison_type_;
};

// Off places specified piece on the current position in game state.
class Off : public IndexedMove {
public:
  explicit Off(piece_id_t piece, unsigned int index)
      : IndexedMove(MoveType::kOffType, index), piece_(piece) {}

  piece_id_t piece() const { return piece_; }

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

private:
  piece_id_t piece_;
};

// PlayerSwitch changes the player to the one specified.
class PlayerSwitch : public IndexedMove {
public:
  explicit PlayerSwitch(player_id_t player, unsigned int index)
      : IndexedMove(MoveType::kPlayerSwitchType, index), player_(player) {}

  player_id_t player() const { return player_; }

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

private:
  player_id_t player_;
};

// KeeperSwitch changes the player to the special keeper player
class KeeperSwitch : public IndexedMove {
public:
  explicit KeeperSwitch(player_id_t keeper_id, uint index)
      : IndexedMove(MoveType::kKeeperSwitchType, index), keeper_id_(keeper_id) {
  }

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

  player_id_t keeper_id() const { return keeper_id_; }

private:
  player_id_t keeper_id_;
};

// Assignment assigns a value to the variable.
class Assignment : public IndexedMove {
public:
  Assignment(size_t variable, std::unique_ptr<ArithmeticOperation> value,
             uint index)
      : IndexedMove(MoveType::kAssignmentType, index), variable_(variable),
        value_(std::move(value)) {}

  variable_id_t get_variable() const { return variable_; }

  const ArithmeticOperation &get_value_expression() const { return *value_; }

  std::unique_ptr<ArithmeticOperation> ExtractValueExpression() {
    return std::move(value_);
  }

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

private:
  variable_id_t variable_;
  std::unique_ptr<ArithmeticOperation> value_;
};

// Condition is true if there is a legal path from initial to final.
class Condition : public IndexedMove {
public:
  Condition(std::unique_ptr<Nfa<std::unique_ptr<Move>>> nfa, bool negated,
            uint index)
      : IndexedMove(MoveType::kConditionCheck, index), nfa_(std::move(nfa)),
        negated_(negated) {}

  const Nfa<std::unique_ptr<Move>> &nfa() const { return *nfa_; }

  Nfa<std::unique_ptr<Move>> &nfa() { return *nfa_; }

  bool negated() const { return negated_; }

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

private:
  std::unique_ptr<Nfa<std::unique_ptr<Move>>> nfa_;
  bool negated_;
};

class PlayerCheck : public IndexedMove {
public:
  explicit PlayerCheck(player_id_t player, uint index)
      : IndexedMove(MoveType::kPlayerCheck, index), player_(player) {}

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

  player_id_t player() const { return player_; }

private:
  player_id_t player_;
};

class VisitedQuery : public Move {
public:
  explicit VisitedQuery() : Move(MoveType::kVisitedCheck) {}

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }
};

class EmptyBackward : public Move {
public:
  EmptyBackward() : Move(MoveType::kBackwardEmpty) {}

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }
};

class EmptyForward : public Move {
public:
  EmptyForward() : Move(MoveType::kForwardEmpty) {}

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }
};

class Noop : public Move {
public:
  Noop() : Move(MoveType::kNoop) {}

  void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }
};
} // namespace rbg

#endif // RBGGAMEMANAGER_MOVES_H
