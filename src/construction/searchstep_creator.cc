//
// Created by shrum on 23.06.19.
//

#include <queue>
#include "searchstep_creator.h"
#include "moves/arithmetic_creator.h"

using namespace rbg;
using namespace std;

struct InitialFinalIndiciesPair {
  uint initial, final;
};

InitialFinalIndiciesPair CreateNfaSearchSteps(SearchStepsCollection &collection, const Nfa<std::unique_ptr<Move>> &nfa);

class SearchStepCreator : public MoveFunction<uint> {
public:
  explicit SearchStepCreator(SearchStepsCollection &collection)
      : collection_(collection) {
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
    assert(false);
  }

  uint OffCase(const Off &move) override {
    uint step_index = collection_.AddSearchStep(make_unique<OffStep>(move.piece(), move.index()));
    collection_.RegisterModifier(move.index(), step_index);
    return step_index;

  }

  uint OnCase(const On &move) override {
    return collection_.AddSearchStep(make_unique<OnStep>(move.pieces_table()));

  }

  uint PlayerSwitchCase(const PlayerSwitch &move) override {
    uint step_index = collection_.AddSearchStep(make_unique<PlayerSwitchStep>(move.player(), move.index()));
    collection_.RegisterModifier(move.index(), step_index);
    return step_index;
  }

  uint KeeperSwitchCase(const KeeperSwitch &move) override {
    uint step_index = collection_.AddSearchStep(make_unique<PlayerSwitchStep>(move.keeper_id(), move.index()));
    collection_.RegisterModifier(move.index(), step_index);
    return step_index;
  }

  uint AssignmentCase(const Assignment &move) override {
    uint step_index = collection_.AddSearchStep(make_unique<AssignmentStep>(move.get_variable(),
                                                                            CreateArithmeticOperation(
                                                                           move.get_value_expression()), move.index()));
    collection_.RegisterModifier(move.index(), step_index);
    return step_index;
  }

  uint PlayerCheckCase(const PlayerCheck &move) override {
    return collection_.AddSearchStep(make_unique<PlayerCheckStep>(move.player()));
  }

  uint ConditionCheckCase(const ConditionCheck &move) override {
    InitialFinalIndiciesPair initial_final_indices = CreateNfaSearchSteps(collection_, move.nfa());
    return collection_.AddSearchStep(make_unique<ConditionCheckStep>(collection_[initial_final_indices.initial]));

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
};

InitialFinalIndiciesPair CreateNfaSearchSteps(SearchStepsCollection &collection, const Nfa<unique_ptr<Move>> &nfa) {
  unordered_map<node_t, uint> nodes_collection_indices;
  queue<node_t> to_visit;
  to_visit.push(nfa.final);
  nodes_collection_indices[nfa.final] = collection.AddSearchStep(make_unique<EndStep>());
  while (!to_visit.empty()) {
    uint v = to_visit.front();
    to_visit.pop();
    auto transitions = nfa.graph.InTransitions(v);
    for (const auto &transition : transitions) {
      uint u = transition.from;
      uint searchstep_index;
      auto iter = nodes_collection_indices.find(u);
      if (nodes_collection_indices.find(u) == nodes_collection_indices.end()) {
        searchstep_index = SearchStepCreator(collection)(*transition.content);
        nodes_collection_indices[u] = searchstep_index;
        to_visit.push(u);
      } else {
        searchstep_index = iter->second;
      }
      collection[searchstep_index]->AddNextSearchStep(collection[nodes_collection_indices.at(v)]);
    }
  }
  return {nodes_collection_indices.at(nfa.initial), nodes_collection_indices.at(nfa.final)};
}

SearchStepsCollection rbg::CreateSearchSteps(const NfaWithVisitedChecks &game_graph, const Declarations &declarations) {
  SearchStepsCollection result(game_graph.visited_checks_count, declarations.board_description.vertices_count());
  auto initial_final_indices = CreateNfaSearchSteps(result, game_graph.nfa);
  result.SetInitial(initial_final_indices.initial);
  return result;
}