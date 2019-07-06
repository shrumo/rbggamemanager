//
// Created by shrum on 23.06.19.
//

#include <queue>
#include "search_step_creator.h"
#include "moves/arithmetic_creator.h"

using namespace rbg;
using namespace std;

struct InitialFinalIndiciesPair {
  uint initial, final;
};

SearchStepsPoint
CreateStepsInCollection(const Nfa<std::unique_ptr<Move>> &game_graph, SearchStepsCollection &collection,
                        const Declarations &declarations);

class SearchStepCreator : public MoveFunction<uint> {
public:
  explicit SearchStepCreator(SearchStepsCollection &collection, const Declarations &declarations,
                             bool register_modifiers = true)
      : collection_(collection), declarations_(declarations), register_modifiers_(register_modifiers) {
  }

  uint ShiftCase(const Shift &move) override {
    uint index = collection_.AddSearchStep(make_unique<ShiftStep>(move.edge_id()));
    return index;
  }

  uint ShiftTableCase(const ShiftTable &) override {
    assert(false && "not yet implemented");
  }

  uint ArithmeticComparisonCase(const ArithmeticComparison &move) override {
    auto left_result = CreateArithmeticOperation(move.left());
    auto right_result = CreateArithmeticOperation(move.right());
    switch (move.comparison_type()) {
      case ComparisonType::kEqual:
        return collection_.AddSearchStep(make_unique<ArithmeticEqualComparison>(
            std::move(left_result), std::move(right_result)));
      case ComparisonType::kNotEqual:
        return collection_.AddSearchStep(make_unique<ArithmeticNotEqualComparison>(
            std::move(left_result),
            std::move(right_result)
        ));
      case ComparisonType::kLess:
        return collection_.AddSearchStep(make_unique<ArithmeticLessComparison>(
            std::move(left_result),
            std::move(right_result)
        ));
      case ComparisonType::kLessEqual:
        return collection_.AddSearchStep(make_unique<ArithmeticLessEqualComparison>(
            std::move(left_result),
            std::move(right_result)
        ));
      case ComparisonType::kGreater:
        return collection_.AddSearchStep(make_unique<ArithmeticLessComparison>(
            std::move(right_result),
            std::move(left_result)
        ));
      case ComparisonType::kGreaterEqual:
        return collection_.AddSearchStep(make_unique<ArithmeticLessEqualComparison>(
            std::move(right_result),
            std::move(left_result)
        ));
    }
  }

  uint OffCase(const Off &move) override {
    uint step_index = collection_.AddSearchStep(make_unique<OffStep>(move.piece(), move.index()));
    if (register_modifiers_) {
      collection_.RegisterModifier(move.index(), step_index);
    }
    return step_index;

  }

  uint OnCase(const On &move) override {
    return collection_.AddSearchStep(make_unique<OnStep>(move.pieces_table()));

  }

  uint PlayerSwitchCase(const PlayerSwitch &move) override {
    uint step_index = collection_.AddSearchStep(make_unique<PlayerSwitchStep>(move.player(), move.index()));
    if (register_modifiers_) {
      collection_.RegisterModifier(move.index(), step_index);
    }
    return step_index;
  }

  uint KeeperSwitchCase(const KeeperSwitch &move) override {
    uint step_index = collection_.AddSearchStep(make_unique<PlayerSwitchStep>(move.keeper_id(), move.index()));
    if (register_modifiers_) {
      collection_.RegisterModifier(move.index(), step_index);
    }
    return step_index;
  }

  uint AssignmentCase(const Assignment &move) override {
    uint step_index = collection_.AddSearchStep(make_unique<AssignmentStep>(move.get_variable(),
                                                                            CreateArithmeticOperation(
                                                                           move.get_value_expression()), move.index()));
    if (register_modifiers_) {
      collection_.RegisterModifier(move.index(), step_index);
    }
    return step_index;
  }

  uint PlayerCheckCase(const PlayerCheck &move) override {
    return collection_.AddSearchStep(make_unique<PlayerCheckStep>(move.player()));
  }

  uint ConditionCheckCase(const ConditionCheck &move) override {
    if (move.negated()) {
      return collection_.AddSearchStep(make_unique<NegatedConditionCheckStep>(
          make_unique<SearchStepsPoint>(CreateStepsInCollection(move.nfa(), collection_, declarations_))));
    } else {
      return collection_.AddSearchStep(make_unique<ConditionCheckStep>(
          make_unique<SearchStepsPoint>(CreateStepsInCollection(move.nfa(), collection_, declarations_))));
    }
  }

  uint VisitedCheckCase(const VisitedCheck &move) override {
    return collection_.AddSearchStep(
        make_unique<VisitedCheckSearchStep>(collection_.GetBitArrayChunk(move.visited_array_index())));

  }

  uint EmptyCase(const Empty &) override {
    return collection_.AddSearchStep(make_unique<MultipleSearchStep>());

  }

private:
  SearchStepsCollection &collection_;
  const Declarations &declarations_;
  bool register_modifiers_;
};

SearchStepsPoint
CreateStepsInCollection(const Nfa<std::unique_ptr<Move>> &game_graph, SearchStepsCollection &collection,
                        const Declarations &declarations) {
  const auto &nfa = game_graph;
  unordered_map<node_t, uint> nodes_collection_indices;
  queue<node_t> to_visit;
  unordered_set<node_t> initialized;
  to_visit.push(nfa.initial);
  initialized.insert(nfa.initial);
  while (!to_visit.empty()) {
    uint v = to_visit.front();
    to_visit.pop();
    auto transitions = nfa.graph.Transitions(v);
    for (const auto &transition : transitions) {
      uint u = transition.to;
      if (initialized.find(u) == initialized.end()) {
        initialized.insert(u);
        to_visit.push(u);
      }
      if (nodes_collection_indices.find(v) == nodes_collection_indices.end()) {
        nodes_collection_indices[v] = SearchStepCreator(collection, declarations)(*transition.content);
      }
    }
  }
  nodes_collection_indices[nfa.final] = collection.AddSearchStep(make_unique<EndStep>());

  to_visit = {};
  to_visit.push(nfa.final);
  unordered_set<node_t> visited;
  while (!to_visit.empty()) {
    uint v = to_visit.front();
    to_visit.pop();
    auto transitions = nfa.graph.InTransitions(v);
    for (const auto &transition : transitions) {
      uint u = transition.from;
      if (visited.find(u) == visited.end()) {
        to_visit.push(u);
        visited.insert(u);
      }
      collection[nodes_collection_indices.at(u)]->AddNextSearchStep(collection[nodes_collection_indices.at(v)]);
    }
  }
  return SearchStepsPoint{collection, collection[nodes_collection_indices.at(nfa.initial)]};
}

SearchStepsInformation
rbg::CreateSearchSteps(const NfaWithVisitedChecks &game_graph, const Declarations &declarations) {
  SearchStepsCollection collection(game_graph.visited_checks_count, declarations.board_description.vertices_count());
  auto point = CreateStepsInCollection(game_graph.nfa, collection, declarations);
  return SearchStepsInformation{std::move(collection), point};
}