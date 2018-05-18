//
// Created by shrum on 19/02/18.
//

#ifndef RBGGAMEMANAGER_ACTION_TYPES_H
#define RBGGAMEMANAGER_ACTION_TYPES_H

enum class ActionType {
  // Switches
      kSwitchType,

  kSwitchesEndMarker,

  // Modifiers
      kOffType,
  kAssignmentType,

  kModifiersEndMarker,

  // Others
      kEmptyType,
  kShiftType,
  kOnType,
  kConditionCheckType,
  kNegatedConditionCheckType,

  kArithmeticLessComparisonType,
  kArithmeticLessEqualComparisonType,
  kArithmeticEqualComparisonType,
  kArithmeticNotEqualComparisonType,

  kPlayerCheck,

  kEmptyGreedy,
};

constexpr bool IsSwitchType(ActionType type) {
  return type < ActionType::kSwitchesEndMarker;
}

constexpr bool IsModifierType(ActionType type) {
  return type < ActionType::kModifiersEndMarker;
}

enum class ArithmeticOperationType : char {
  kVariableType,
  kConstantType,
  kSumType,
  kProductType,
};

#endif //RBGGAMEMANAGER_ACTION_TYPES_H
