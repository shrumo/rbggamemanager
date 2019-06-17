//
// Created by shrum on 16.06.19.
//

#ifndef RBGGAMEMANAGER_MOVES_H
#define RBGGAMEMANAGER_MOVES_H


#include <gamestate/board.h>
#include <gamestate/searchsteps/searchstep.h>
#include <graph/graph.h>

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

  class ConditionCheck;

  enum class MoveType {
    kShiftType,
    kShiftTableType,
    kArithmeticComparisonType,
    kOffType,
    kOnType,
    kPlayerSwitchType,
    kKeeperSwitchType,
    kAssignmentType,
    kPlayerCheck,
    kConditionCheck
  };

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

    virtual void Visit(const ConditionCheck &) = 0;
  };

  class Move {
  public:
    explicit Move(MoveType type, uint index) : type_(type), index_(index) {}

    MoveType type() const {
      return type_;
    }

    uint index() const {
      return index_;
    }

    virtual void Accept(MoveVisitor &visitor) const = 0;

  private:
    MoveType type_;
    uint index_;
  };

  // Shift changes the current position on board.
  class Shift : public Move {
  public:
    explicit Shift(shift_edge_id_t edge_id, uint index) : Move(MoveType::kShiftType, index), edge_id_(edge_id) {}

    // Returns the edge this shift travels.
    shift_edge_id_t edge_id() const { return edge_id_; }

    void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

  private:
    shift_edge_id_t edge_id_;
  };

  // Shift changes the current position on board.
  class ShiftTable : public Move {
  public:
    explicit ShiftTable(std::vector<std::vector<vertex_id_t> > table, uint index) : Move(MoveType::kShiftTableType,
                                                                                         index),
                                                                                    table_(std::move(table)) {
      for (auto &row : table_) {
        std::vector<vertex_id_t> new_row;
        new_row.reserve(row.size());
        for (long element : row) {
          if (element != -1) new_row.push_back(element);
        }
        row = std::move(new_row);
        row.shrink_to_fit();
      }
    }

    // Returns the edge this shift travels.
    const std::vector<std::vector<vertex_id_t> > &table() const { return table_; }


    void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

  private:
    std::vector<std::vector<vertex_id_t> > table_;
  };

  // On checks if one of the pieces is on the current position on board.
  class On : public Move {
  public:
    // Parameter pieces should contain true for token ids of pieces names that the on accepts.
    explicit On(std::vector<bool> pieces, uint index)
        : Move(MoveType::kOnType, index), pieces_(std::move(pieces)) {}

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
  class ArithmeticComparison : public Move {
  public:
    ArithmeticComparison(std::unique_ptr<ArithmeticOperation> left,
                         std::unique_ptr<ArithmeticOperation> right, ComparisonType type, uint index)
        : Move(MoveType::kArithmeticComparisonType, index),
          left_(std::move(left)),
          right_(std::move(right)),
          comparison_type_(type) {}

    const ArithmeticOperation *left() const {
      return left_.get();
    }

    const ArithmeticOperation *right() const {
      return right_.get();
    }

    std::unique_ptr<ArithmeticOperation> ExtractLeft() {
      return std::move(left_);
    }

    std::unique_ptr<ArithmeticOperation> ExtractRight() {
      return std::move(right_);
    }

    ComparisonType comparison_type() const {
      return comparison_type_;
    }

    void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

  private:
    std::unique_ptr<ArithmeticOperation> left_, right_;
    ComparisonType comparison_type_;
  };

  // Off places specified piece on the current position in game state.
  class Off : public Move {
  public:
    explicit Off(piece_id_t piece, unsigned int index = 0) : Move(
        MoveType::kOffType, index), piece_(piece) {}

    piece_id_t piece() const { return piece_; }


    void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

  private:
    piece_id_t piece_;
  };

  // PlayerSwitch changes the player to the one specified.
  class PlayerSwitch : public Move {
  public:
    explicit PlayerSwitch(player_id_t player, unsigned int index = 0) : Move(
        MoveType::kPlayerSwitchType, index), player_(player) {}

    player_id_t player() const { return player_; }


    void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

  private:
    player_id_t player_;
  };

  // PlayerSwitch changes the player to the one specified.
  class KeeperSwitch : public Move {
  public:
    explicit KeeperSwitch(uint index) : Move(
        MoveType::kKeeperSwitchType, index) {}

    void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }
  };

  // Assignment assigns a value to the variable.
  class Assignment : public Move {
  public:
    Assignment(size_t variable, std::unique_ptr<ArithmeticOperation> value,
               uint index)
        : Move(MoveType::kAssignmentType, index),
          variable_(variable),
          value_(std::move(value)) {}

    variable_id_t get_variable() const { return variable_; }

    const ArithmeticOperation *get_value_expression() const {
      return value_.get();
    }

    std::unique_ptr<ArithmeticOperation> ExtractValueExpression() {
      return std::move(value_);
    }

    void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

  private:
    variable_id_t variable_;
    std::unique_ptr<ArithmeticOperation> value_;
  };

  class ConditionCheck : public Move {
  public:
    ConditionCheck(rbg::Nfa<std::unique_ptr<Move>> nfa, bool negated, uint index)
        : Move(MoveType::kConditionCheck, index), nfa_(std::move(nfa)), negated_(negated) {
    }

    const rbg::Nfa<std::unique_ptr<Move>> &nfa() const { return nfa_; }

    bool negated() const { return negated_; }

    void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

  private:
    rbg::Nfa<std::unique_ptr<Move>> nfa_;
    bool negated_;
  };

  class PlayerCheck : public Move {
  public:
    explicit PlayerCheck(player_id_t player, uint index)
        : Move(MoveType::kPlayerCheck, index), player_(player) {
    }

    void Accept(MoveVisitor &visitor) const override { visitor.Visit(*this); }

    player_id_t player() const {
      return player_;
    }

  private:
    player_id_t player_;
  };


}

#endif //RBGGAMEMANAGER_MOVES_H
