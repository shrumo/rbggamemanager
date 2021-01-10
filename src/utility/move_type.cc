#include <utility/move_type.h>

#include "parser/parser_wrapper.h"

namespace rbg {

class GetNodeTypeFn : public AstFunction<NodeType> {
 public:
  NodeType SumCase(const rbg_parser::sum &) override { return NodeType::kSum; }
  NodeType ConcatenationCase(const rbg_parser::concatenation &) override {
    return NodeType::kConcat;
  }
  NodeType StarCase(const rbg_parser::star_move &) override {
    return NodeType::kStar;
  }
  NodeType GameMoveCase(const rbg_parser::game_move &) {
    return NodeType::kMove;
  }
};

class GetMoveTypeFn : public AstFunction<MoveType> {
 public:
  MoveType ShiftCase(const rbg_parser::shift &) override {
    return MoveType::kShiftType;
  }

  MoveType OnCase(const rbg_parser::ons &) override {
    return MoveType::kOnType;
  }

  MoveType OffCase(const rbg_parser::off &) override {
    return MoveType::kOffType;
  }

  MoveType AssignmentCase(const rbg_parser::assignment &) override {
    return MoveType::kAssignmentType;
  }

  MoveType PlayerSwitchCase(const rbg_parser::player_switch &) override {
    return MoveType::kPlayerSwitchType;
  }

  MoveType KeeperSwitchCase(const rbg_parser::keeper_switch &) override {
    return MoveType::kKeeperSwitchType;
  }

  MoveType MoveCheckCase(const rbg_parser::move_check &) override {
    return MoveType::kConditionCheck;
  }

  MoveType ArithmeticComparisonCase(
      const rbg_parser::arithmetic_comparison &) override {
    return MoveType::kArithmeticComparisonType;
  }
};

NodeType ParserNodeType(const rbg_parser::game_move &move) {
  return GetNodeTypeFn()(move);
}

MoveType ParserMoveType(const rbg_parser::game_move &move) {
  return GetMoveTypeFn()(move);
}

bool IsModifier(MoveType type) {
  switch (type) {
    case MoveType::kOffType:
    case MoveType::kKeeperSwitchType:
    case MoveType::kPlayerSwitchType:
    case MoveType::kAssignmentType:
      return true;
    default:
      return false;
  }
}

bool IsModifier(const rbg_parser::game_move &move) {
  return IsModifier(ParserMoveType(move));
}
}  // namespace rbg