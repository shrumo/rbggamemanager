#ifndef RBGGAMEMANAGER_MOVE_TYPE_H
#define RBGGAMEMANAGER_MOVE_TYPE_H

#include <parser/parser_actions.h>

namespace rbg {
enum class MoveType {
  kShiftType,                // 0
  kShiftTableType,           // 1
  kArithmeticComparisonType, // 2
  kOffType,                  // 3
  kOnType,                   // 4
  kPlayerSwitchType,         // 5
  kKeeperSwitchType,         // 6
  kAssignmentType,           // 7
  kPlayerCheck,              // 8
  kConditionCheck,           // 9
  kVisitedCheck,             // 10
  kBackwardEmpty,            // 11
  kForwardEmpty,             // 12
  kNoop                      // 13
};

enum class NodeType { kMove, kSum, kStar, kConcat };

NodeType ParserNodeType(const rbg_parser::game_move &move);
MoveType ParserMoveType(const rbg_parser::game_move &move);
bool IsModifier(MoveType type);
bool IsModifier(const rbg_parser::game_move &move);
bool IsSwitch(MoveType type);
bool IsSwitch(const rbg_parser::game_move &move);

} // namespace rbg

#endif // RBGGAMEMANAGER_MOVE_TYPE_H
