//
// Created by shrum on 19/02/18.
//

#ifndef RBGGAMEMANAGER_TYPES_H
#define RBGGAMEMANAGER_TYPES_H

enum class ActionType {
  // Switches
      kSwitchType,

  // Modifiers
      kOffType,
  kAssignmentType,

  // Others
      kEmptyType,
  kShiftType,
  kShiftTableType,
  kOnType,
  kConditionCheckType,
  kNegatedConditionCheckType,

  kArithmeticLessComparisonType,
  kArithmeticLessEqualComparisonType,
  kArithmeticEqualComparisonType,
  kArithmeticNotEqualComparisonType,

  kPlayerCheck,
};

constexpr bool IsSwitchType(ActionType type) {
  return type == ActionType::kSwitchType;
}

constexpr bool IsModifierType(ActionType type) {
  return type == ActionType::kSwitchType || type == ActionType::kOffType || type == ActionType::kAssignmentType;
}

enum class ArithmeticOperationType : char {
  kVariableType,
  kConstantType,
  kSumType,
  kSubtractionType,
  kProductType,
  kDivisionType,
};

#endif //RBGGAMEMANAGER_TYPES_H
