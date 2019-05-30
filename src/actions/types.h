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

std::string ActionName(ActionType type) {
  switch (type) {
    case ActionType::kSwitchType:
      return "switch";
    case ActionType::kOffType:
      return "off";
    case ActionType::kAssignmentType:
      return "assignment";
    case ActionType::kEmptyType:
      return "empty";
    case ActionType::kShiftType:
      return "shift";
    case ActionType::kShiftTableType:
      return "shift_table";
    case ActionType::kOnType:
      return "on";
    case ActionType::kConditionCheckType:
      return "condition_check";
    case ActionType::kNegatedConditionCheckType:
      return "negated_condition_check";
    case ActionType::kArithmeticLessComparisonType:
      return "arithmetic_le";
    case ActionType::kArithmeticLessEqualComparisonType:
      return "arithmetic_leq";
    case ActionType::kArithmeticEqualComparisonType:
      return "arithmetic_eq";
    case ActionType::kArithmeticNotEqualComparisonType:
      return "arithmetic_neq";
    case ActionType::kPlayerCheck:
      return "player_check";
  }
}

#endif //RBGGAMEMANAGER_TYPES_H
