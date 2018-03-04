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
  kIncrementationType,
  kDecrementationType,
  kLazyType,

  kModifiersEndMarker,

  // Others
      kEmptyType,
  kShiftType,
  kOnType,
  kConditionType,
};

constexpr bool IsSwitchType(ActionType type) {
  return type < ActionType::kSwitchesEndMarker;
}

constexpr bool IsModifierType(ActionType type) {
  return type < ActionType::kModifiersEndMarker;
}

enum class ConditionType : char {
  kConjunctionType,
  kAlternativeType,
  kNegationType,
  kLessType,
  kLessEqualType,
  kEqualType,
  kNotEqualType,
  kMovePatternType,
};

enum class ArithmeticOperationType : char {
  kVariableType,
  kConstantType,
};

#endif //RBGGAMEMANAGER_ACTION_TYPES_H
