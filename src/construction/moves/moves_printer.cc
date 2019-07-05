//
// Created by shrum on 17.06.19.
//

#include "moves_printer.h"

#include "moves_visitor.h"
#include <game_state/search_steps/arithmetic_operation_visitor.h>
#include "moves.h"
#include <sstream>
#include <construction/graph_creator.h>

using namespace rbg;


class ArithmeticOperationPrinter : public ArithmeticOperationFunction<std::string> {
public:
  explicit ArithmeticOperationPrinter(const Declarations &declarations)
      : declarations_(declarations) {}

  std::string VariableCase(const VariableValue &variable_value) override {
    return declarations_.variables_resolver.Name(variable_value.variable_id());
  }

  std::string ConstantCase(const ConstantValue &constant_value) override {
    return std::to_string(constant_value.value());
  }

  std::string SumCase(const SumValue &sum_value) override {
    std::string result;
    for (size_t i = 0; i < sum_value.summands().size(); i++) {
      if (i != 0) {
        result += "+ ";
      }
      const auto &child = sum_value.summands()[i];
      result += ArithmeticOperationPrinter(declarations_)(*child);
    }
    return result;
  }

  std::string SubtractionCase(const SubtractionValue &subtaction_value) override {
    std::string result;
    for (size_t i = 0; i < subtaction_value.elements().size(); i++) {
      if (i != 0) {
        result += "- ";
      }
      const auto &child = subtaction_value.elements()[i];
      result += ArithmeticOperationPrinter(declarations_)(*child);
    }
    return result;
  }

  std::string ProductCase(const ProductValue &product_value) override {
    std::string result;
    for (size_t i = 0; i < product_value.factors().size(); i++) {
      if (i != 0) {
        result += "* ";
      }
      const auto &child = product_value.factors()[i];
      result += ArithmeticOperationPrinter(declarations_)(*child);
    }
    return result;
  }

  std::string DivisionCase(const DivisionValue &division_value) override {
    std::string result;
    for (size_t i = 0; i < division_value.elements().size(); i++) {
      if (i != 0) {
        result += "/ ";
      }
      const auto &child = division_value.elements()[i];
      result += ArithmeticOperationPrinter(declarations_)(*child);
    }
    return result;
  }

private:
  const Declarations &declarations_;
};

std::string comparison_symbol(ComparisonType type) {
  switch (type) {
    case ComparisonType::kLess:
      return "<";
    case ComparisonType::kLessEqual:
      return "<=";
    case ComparisonType::kEqual:
      return "==";
    case ComparisonType::kNotEqual:
      return "!=";
    case ComparisonType::kGreater:
      return ">";
    case ComparisonType::kGreaterEqual:
      return ">=";
  }
  assert(false && "Not supported type");
}

class MovePrinter : public MoveFunction<std::string> {
public:
  explicit MovePrinter(const Declarations &declarations)
      : declarations_(declarations) {}

  std::string ShiftCase(const Shift &move) override {
    return declarations_.board_description.edges_names().Name(move.edge_id());
  }

  std::string ShiftTableCase(const ShiftTable &move) override {
    std::stringstream stream;
    stream << "<ShiftTable ";
    for (vertex_id_t id = 0; id < move.table().size(); id++) {
      if (id != 0) {
        stream << "; ";
      }
      const auto &v = move.table()[id];
      stream << id << "-> ";
      for (size_t i = 0; i < v.size(); i++) {
        if (i != 0) {
          stream << ", ";
        }
        stream << v[i];
      }
    }
    return stream.str();
  }

  std::string ArithmeticComparisonCase(const ArithmeticComparison &move) override {

    return "{$" + ArithmeticOperationPrinter(declarations_)(move.left()) +
           comparison_symbol(move.comparison_type()) +
           ArithmeticOperationPrinter(declarations_)(move.right()) + "}";
  }

  std::string OffCase(const Off &move) override {
    return "[" + declarations_.pieces_resolver.Name(move.piece()) + "]";
  }

  std::string OnCase(const On &move) override {
    std::string result = "{";
    for (size_t i = 0; i < move.pieces().size(); i++) {
      if (i != 0) {
        result += ", ";
      }
      piece_id_t piece = move.pieces()[i];
      result += declarations_.pieces_resolver.Name(piece);
    }
    result += "}";
    return result;
  }

  std::string PlayerSwitchCase(const PlayerSwitch &move) override {
    return "->" + declarations_.players_resolver.Name(move.player());
  }

  std::string KeeperSwitchCase(const KeeperSwitch &) override {
    return "->>";
  }

  std::string AssignmentCase(const Assignment &move) override {
    return "[" + declarations_.variables_resolver.Name(move.get_variable()) + "= " +
           ArithmeticOperationPrinter(declarations_)(move.get_value_expression()) + "]";
  }

  std::string PlayerCheckCase(const PlayerCheck &move) override {
    return "{" + declarations_.players_resolver.Name(move.player()) + "}";
  }

  std::string ConditionCheckCase(const ConditionCheck &move) override {
    std::stringstream result;
    result << "<ConditionCheck ";
    result << GraphDescription(move.nfa().graph,
                               [&](const std::unique_ptr<Move> &move) { return MovePrinter(declarations_)(*move); });
    result << ">";
    return result.str();
  }

  std::string VisitedCheckCase(const VisitedCheck &move) override {
    return "<VisitedCheck " + std::to_string(move.visited_array_index()) + ">";
  }

  std::string EmptyCase(const Empty &) override {
    return "e";
  }
private:
  const Declarations &declarations_;
};

std::string rbg::MoveDescription(const Move &move, const Declarations &declarations) {
  return MovePrinter(declarations)(move);
}