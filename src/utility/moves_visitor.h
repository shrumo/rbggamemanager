//
// Created by shrum on 17.06.19.
//

#ifndef RBGGAMEMANAGER_MOVES_VISITOR_H
#define RBGGAMEMANAGER_MOVES_VISITOR_H

#include "game_description/moves/moves.h"

namespace rbg {

  template<typename ResultType>
  class MoveFunction : private MoveVisitor {
  public:
    virtual ResultType ShiftCase(const Shift &move) { return DefaultCase(move); }

    virtual ResultType ShiftTableCase(const ShiftTable &move) { return DefaultCase(move); }

    virtual ResultType ArithmeticComparisonCase(const ArithmeticComparison &move) { return DefaultCase(move); }

    virtual ResultType OffCase(const Off &move) { return DefaultCase(move); }

    virtual ResultType OnCase(const On &move) { return DefaultCase(move); }

    virtual ResultType PlayerSwitchCase(const PlayerSwitch &move) { return DefaultCase(move); }

    virtual ResultType KeeperSwitchCase(const KeeperSwitch &move) { return DefaultCase(move); }

    virtual ResultType AssignmentCase(const Assignment &move) { return DefaultCase(move); }

    virtual ResultType PlayerCheckCase(const PlayerCheck &move) { return DefaultCase(move); }

    virtual ResultType ConditionCase(const Condition &move) { return DefaultCase(move); }

    virtual ResultType VisitedQueryCase(const VisitedQuery &move) { return DefaultCase(move); }

    virtual ResultType EmptyCase(const Empty &move) { return DefaultCase(move); }

    virtual ResultType DefaultCase(const Move &) { return ResultType(); }

    template<typename NodeType>
    ResultType operator()(const NodeType &move) {
      move.Accept(*this);
      assert(result_exists_ &&
                 "You cannot reuse the result. Extraction of the result can happen exactly once after calling Visit.");
      result_exists_ = false;
      return std::move(result_);
    }

  private:
    void Visit(const Shift &m) override {
      result_ = ShiftCase(m);
      result_exists_ = true;
    };

    void Visit(const ShiftTable &m) override {
      result_ = ShiftTableCase(m);
      result_exists_ = true;
    };

    void Visit(const ArithmeticComparison &m) override {
      result_ = ArithmeticComparisonCase(m);
      result_exists_ = true;
    };

    void Visit(const Off &m) override {
      result_ = OffCase(m);
      result_exists_ = true;
    };

    void Visit(const On &m) override {
      result_ = OnCase(m);
      result_exists_ = true;
    };

    void Visit(const PlayerSwitch &m) override {
      result_ = PlayerSwitchCase(m);
      result_exists_ = true;
    };

    void Visit(const KeeperSwitch &m) override {
      result_ = KeeperSwitchCase(m);
      result_exists_ = true;
    };

    void Visit(const Assignment &m) override {
      result_ = AssignmentCase(m);
      result_exists_ = true;
    };

    void Visit(const PlayerCheck &m) override {
      result_ = PlayerCheckCase(m);
      result_exists_ = true;
    };

    void Visit(const Condition &m) override {
      result_ = ConditionCase(m);
      result_exists_ = true;
    };

    void Visit(const VisitedQuery &m) override {
      result_ = VisitedQueryCase(m);
      result_exists_ = true;
    };

    void Visit(const Empty &m) override {
      result_ = EmptyCase(m);
      result_exists_ = true;
    };

    bool result_exists_ = false;
    ResultType result_;
  };
}

#endif //RBGGAMEMANAGER_MOVES_VISITOR_H
