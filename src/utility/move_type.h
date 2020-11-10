#ifndef RBGGAMEMANAGER_MOVE_TYPE_H
#define RBGGAMEMANAGER_MOVE_TYPE_H

#include<parser/parser_actions.h>

namespace rbg {
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
    kConditionCheck,
    kVisitedCheck,
    kBackwardEmpty,
    kForwardEmpty
  };

  enum class NodeType {
    kMove,
    kSum,
    kStar,
    kConcat
  };

  NodeType ParserNodeType(const rbg_parser::game_move& move);

 } // namespace rbg

 #endif // RBGGAMEMANAGER_MOVE_TYPE_H
