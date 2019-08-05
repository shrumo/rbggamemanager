//
// Created by shrum on 17.06.19.
//

#include "printer.h"

#include "utility/moves_visitor.h"
#include "utility/arithmetic_operation_visitor.h"
#include "game_description/moves/moves.h"
#include <sstream>
#include <iomanip>
#include "game_description/construction/graph_creator.h"
#include <parser/parser_wrapper.h>
#include "game_state/game_state.h"

using namespace rbg;


class ArithmeticOperationPrinter : public ArithmeticOperationFunction<std::string> {
public:
  explicit ArithmeticOperationPrinter(const Declarations &declarations)
      : declarations_(declarations) {}

  std::string VariableCase(const VariableValue &variable_value) override {
    return declarations_.variables_resolver().Name(variable_value.variable_id());
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
    return declarations_.initial_board().edges_names().Name(move.edge_id());
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
    return "[" + declarations_.pieces_resolver().Name(move.piece()) + "]";
  }

  std::string OnCase(const On &move) override {
    std::string result = "{";
    for (size_t i = 0; i < move.pieces().size(); i++) {
      if (i != 0) {
        result += ", ";
      }
      piece_id_t piece = move.pieces()[i];
      result += declarations_.pieces_resolver().Name(piece);
    }
    result += "}";
    return result;
  }

  std::string PlayerSwitchCase(const PlayerSwitch &move) override {
    return "->" + declarations_.players_resolver().Name(move.player());
  }

  std::string KeeperSwitchCase(const KeeperSwitch &) override {
    return "->>";
  }

  std::string AssignmentCase(const Assignment &move) override {
    return "[" + declarations_.variables_resolver().Name(move.get_variable()) + "= " +
           ArithmeticOperationPrinter(declarations_)(move.get_value_expression()) + "]";
  }

  std::string PlayerCheckCase(const PlayerCheck &move) override {
    return "{" + declarations_.players_resolver().Name(move.player()) + "}";
  }

  std::string ConditionCase(const Condition &move) override {
    std::stringstream result;
    result << "<ConditionCheck ";
    result << GraphDescription(move.nfa().graph,
                               [&](const std::unique_ptr<Move> &move) { return MovePrinter(declarations_)(*move); });
    result << ">";
    return result.str();
  }

  std::string VisitedQueryCase(const VisitedQuery &move) override {
    return "<VisitedQuery " + std::to_string(move.visited_array_index()) + ">";
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

std::unordered_map<uint, std::string> rbg::ActionsDescriptionsMap(const std::string &game_text) {
  std::unordered_map<uint, std::string> result;
  auto game = ParseGame(ParseGame(game_text)->to_rbg());
  Declarations declarations = Declarations(*game);
  VisitedChecksNfa nfa = CreateVisitedChecksNfa(*game->get_moves(), declarations);
  for(node_t node: nfa.nfa.graph.nodes()) {
    for(const auto& edge : nfa.nfa.graph.EdgesFrom(node)) {
      if(edge.content()->indexed()) {
        const auto& indexed_edge = dynamic_cast<const IndexedMove&>(*edge.content());
        result[indexed_edge.index()] = MoveDescription(*edge.content(), declarations);
      }
    }
  }
  return result;
}

std::string rbg::RectangularBoardDescription(const BoardContent &board_content, const Declarations &declarations) {
  size_t width = 0;
  size_t height = 0;
  for (vertex_id_t v = 1; v < static_cast<ssize_t >(declarations.initial_board().vertices_count()); v++) {
    std::string vertex_name = declarations.initial_board().vertices_names().Name(v);
    std::stringstream stream(vertex_name);
    char placeholders[3] = {'\0', '\0', '\0'};
    stream >> placeholders[0] >> placeholders[1];
    assert(std::string(placeholders) == "rx" && "The board should be created by rectangular keyword.");
    size_t x, y;
    char placeholder;
    stream >> x >> placeholder >> y;
    assert(placeholder == 'y' && "The board should be created by rectangular keyword.");
    width = std::max(x + 1, width);
    height = std::max(y + 1, height);
  }
  std::vector<piece_id_t> field(width * height);
  for (vertex_id_t v = 1; v < static_cast<ssize_t >(declarations.initial_board().vertices_count()); v++) {
    std::string vertex_name = declarations.initial_board().vertices_names().Name(v);
    std::stringstream stream(vertex_name);
    char placeholder;
    stream >> placeholder >> placeholder;
    size_t x, y;
    stream >> x >> placeholder >> y;
    field[x + y * width] = board_content.at(v);
  }
  std::stringstream s;
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      std::string name = declarations.pieces_resolver().Name(field[x + y * width]);
      if (name == "empty" || name == "e") {
        s << "[" << std::setw(3) << " " << "] ";
      } else {
        s << "[" << std::setw(3) << name.substr(0, (unsigned long) 3)
          << "] ";
      }
    }
    s << '\n';
  }
  return s.str();
}

std::string rbg::VariablesValuesDescription(const GameState &state) {
  std::stringstream s;
  for (variable_id_t id = 0; id < state.declarations().variables_resolver().size(); id++) {
          auto variable_name = state.declarations().variables_resolver().Name(id);
          s << "\t" << variable_name << " (" << id << ") : " << state.variables_values()[id];
          if (state.declarations().players_resolver().contains(variable_name)) {
            s << " (result for player " << variable_name << " ("
                      << state.declarations().players_resolver().Id(variable_name) << "))";
          }
          s << std::endl;
        }
  return s.str();
}
