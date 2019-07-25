//
// Created by shrum on 06.07.19.
//

#include "moves.h"

using namespace rbg;

bool rbg::is_modifier_type(MoveType type) {
  switch (type) {
    case MoveType::kOffType:
    case MoveType::kPlayerSwitchType:
    case MoveType::kKeeperSwitchType:
    case MoveType::kAssignmentType:
      return true;
    default:
      return false;
  }
}