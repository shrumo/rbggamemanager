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

InitialFinalIndiciesPair CreateNfaSearchSteps(SearchstepsCollection &collection, const Nfa<std::unique_ptr<Move>> &nfa);

class SearchstepCreator : public MoveFunction<uint> {
public:
  explicit SearchstepCreator(SearchstepsCollection &collection)
      : collection_(collection) {
  }

  uint ShiftCase(const Shift &move) override {
    uint index = collection_.AddSearchStep(make_unique<ShiftStep>(move.edge_id()));
    return index;
  }

  uint ShiftTableCase(const ShiftTable &move) override {
    assert(false && "not yet implemented");
  }

  uint ArithmeticComparisonCase(const ArithmeticComparison &move) override {
    uint index;
    auto left_result = CreateArithmeticOperation(move.left());
    auto right_result = CreateArithmeticOperation(move.right());
    switch (move.comparison_type()) {
      case ComparisonType::kEqual:
        index = collection_.AddSearchStep(make_unique<ArithmeticEqualComparison>(
            std::move(left_result), std::move(right_result)));
        break;
      case ComparisonType::kNotEqual:
        index = collection_.AddSearchStep(make_unique<ArithmeticNotEqualComparison>(
            std::move(left_result),
            std::move(right_result)
        ));
        break;
      case ComparisonType::kLess:
        index = collection_.AddSearchStep(make_unique<ArithmeticLessComparison>(
            std::move(left_result),
            std::move(right_result)
        ));
        break;
      case ComparisonType::kLessEqual:
        index = collection_.AddSearchStep(make_unique<ArithmeticLessEqualComparison>(
            std::move(left_result),
            std::move(right_result)
        ));
        break;
      case ComparisonType::kGreater:
        index = collection_.AddSearchStep(make_unique<ArithmeticLessComparison>(
            std::move(right_result),
            std::move(left_result)
        ));
        break;
      case ComparisonType::kGreaterEqual:
        index = collection_.AddSearchStep(make_unique<ArithmeticLessEqualComparison>(
            std::move(right_result),
            std::move(left_result)
        ));
        break;
    }
    return index;
  }

  uint OffCase(const Off &move) override {
    uint index = collection_.AddSearchStep(make_unique<OffStep>(move.piece(), move.index()));
    return index;
  }

  uint OnCase(const On &move) override {
    uint index = collection_.AddSearchStep(make_unique<OnStep>(move.pieces_table()));
    return index;
  }

  uint PlayerSwitchCase(const PlayerSwitch &move) override {
    uint index = collection_.AddSearchStep(make_unique<PlayerSwitchStep>(move.player(), move.index()));
    return index;
  }

  uint KeeperSwitchCase(const KeeperSwitch &move) override {
    assert(false && "not yet implemented");
  }

  uint AssignmentCase(const Assignment &move) override {
    uint index = collection_.AddSearchStep(make_unique<AssignmentStep>(move.get_variable(),
                                                                       CreateArithmeticOperation(
                                                                           move.get_value_expression()), move.index()));
    return index;
  }

  uint PlayerCheckCase(const PlayerCheck &move) override {
    assert(false && "not yet implemented");
  }

  uint ConditionCheckCase(const ConditionCheck &move) override {
    InitialFinalIndiciesPair initial_final_indices = CreateNfaSearchSteps(collection_, move.nfa());
    uint index = collection_.AddSearchStep(make_unique<ConditionCheckStep>(collection_[initial_final_indices.initial]));
    return index;
  }

  uint VisitedCheckCase(const VisitedCheck &move) override {
    uint index = collection_.AddSearchStep(
        make_unique<VisitedCheckSearchStep>(collection_.GetBitArrayChunk(move.visited_array_index())));
    return index;
  }

  uint EmptyCase(const Empty &) override {
    uint index = collection_.AddSearchStep(make_unique<MultipleSearchStep>());
    return index;
  }

private:
  SearchstepsCollection &collection_;
};

InitialFinalIndiciesPair CreateNfaSearchSteps(SearchstepsCollection &collection, const Nfa<unique_ptr<Move>> &nfa) {
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
        searchstep_index = SearchstepCreator(collection)(*transition.content);
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

SearchstepsCollection rbg::CreateSearchSteps(const NfaWithVisitedChecks &game_graph, const Declarations &declarations) {
  SearchstepsCollection result(game_graph.visited_checks_count, declarations.board_description.vertices_count());
  auto initial_final_indices = CreateNfaSearchSteps(result, game_graph.nfa);
  result.SetInitial(initial_final_indices.initial);
  return result;
}