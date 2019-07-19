//
// Created by shrum on 23.06.19.
//

#include <queue>
#include <game_state/search_steps/actions.h>
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
    uint index = collection_.AddSearchStep(make_unique<Block<ShiftAction, OutOfBoundsTest, BranchSingle>>(ShiftAction(move.edge_id()), OutOfBoundsTest{}, BranchSingle{}));
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
        return collection_.AddSearchStep(make_unique<Block<EqualComparisonTest, BranchSingle>>(EqualComparisonTest(
            std::move(left_result), std::move(right_result)), BranchSingle{}));
      case ComparisonType::kNotEqual:
        return collection_.AddSearchStep(
            make_unique<Block<NotEqualComparisonTest, BranchSingle>>(NotEqualComparisonTest(
                std::move(left_result),
                std::move(right_result)
            ), BranchSingle{}));
      case ComparisonType::kLess:
        return collection_.AddSearchStep(make_unique<Block<LessComparisonTest, BranchSingle>>(LessComparisonTest(
            std::move(left_result),
            std::move(right_result)
        ), BranchSingle{}));
      case ComparisonType::kLessEqual:
        return collection_.AddSearchStep(
            make_unique<Block<LessEqualComparisonTest, BranchSingle>>(LessEqualComparisonTest(
                std::move(left_result),
                std::move(right_result)), BranchSingle{}));
      case ComparisonType::kGreater:
        return collection_.AddSearchStep(make_unique<Block<LessComparisonTest, BranchSingle>>(LessComparisonTest(
            std::move(right_result),
            std::move(left_result)
        ), BranchSingle{}));
      case ComparisonType::kGreaterEqual:
        return collection_.AddSearchStep(
            make_unique<Block<LessEqualComparisonTest, BranchSingle>>(LessEqualComparisonTest(
                std::move(right_result),
                std::move(left_result)
            ), BranchSingle{}));
    }
    assert(false && "Not possible.");
    return 0;
  }

  uint OffCase(const Off &move) override {
    uint step_index = collection_.AddSearchStep(make_unique<Block<OffApplication, BranchSingle>>(OffApplication(move.piece(), move.index()),BranchSingle{}));
    if (register_modifiers_) {
      collection_.RegisterModifier(move.index(),
                                   dynamic_cast<Block<OffApplication, BranchSingle> *>(collection_[step_index])->GetAction<0>());
    }
    return step_index;
  }

  uint OnCase(const On &move) override {
    return collection_.AddSearchStep(make_unique<Block<OnTest, BranchSingle>>(OnTest(move.pieces_table()), BranchSingle{}));

  }

  uint PlayerSwitchCase(const PlayerSwitch &move) override {
    uint step_index = collection_.AddSearchStep(make_unique<Block<PlayerSwitchApplication, BranchSingle>>(PlayerSwitchApplication(move.player(), move.index()), BranchSingle{}));
    if (register_modifiers_) {
      collection_.RegisterModifier(move.index(),
                                   dynamic_cast<Block<PlayerSwitchApplication, BranchSingle> *>(collection_[step_index])->GetAction<0>());

    }

    uint switch_index = collection_.AddSearchStep(make_unique<BlockPointer<BranchSingle>>(BlockPointer<BranchSingle>(
        dynamic_cast<Block<PlayerSwitchApplication, BranchSingle> *>(collection_[step_index])->GetSubAbstractBlock<1>())));
      collection_.RegisterSwitch(move.index(),collection_[switch_index]);
    return step_index;
  }

  uint KeeperSwitchCase(const KeeperSwitch &move) override {
     uint step_index = collection_.AddSearchStep(make_unique<Block<PlayerSwitchApplication, BranchSingle>>(PlayerSwitchApplication(move.keeper_id(), move.index()), BranchSingle{}));
    if (register_modifiers_) {

      collection_.RegisterModifier(move.index(),
                                   dynamic_cast<Block<PlayerSwitchApplication, BranchSingle> *>(collection_[step_index])->GetAction<0>());
    }

    uint switch_index = collection_.AddSearchStep(make_unique<BlockPointer<BranchSingle>>(BlockPointer<BranchSingle>(
        dynamic_cast<Block<PlayerSwitchApplication, BranchSingle> *>(collection_[step_index])->GetSubAbstractBlock<1>())));
      collection_.RegisterSwitch(move.index(),collection_[switch_index]);
    return step_index;
  }

  uint AssignmentCase(const Assignment &move) override {
    uint step_index = collection_.AddSearchStep(make_unique<Block<AssignmentAction, VariableBoundsTest, BranchSingle>>(AssignmentAction(move.get_variable(),
                                                                            CreateArithmeticOperation(
                                                                           move.get_value_expression()), move.index()), VariableBoundsTest(move.get_variable()), BranchSingle{}));
    if (register_modifiers_) {
      auto tmp = dynamic_cast<Block<AssignmentAction, VariableBoundsTest, BranchSingle> *>(collection_[step_index]);
      collection_.RegisterModifier(move.index(),
                                   tmp->GetAction<0>());
    }
    return step_index;
  }

  uint PlayerCheckCase(const PlayerCheck &move) override {
    return collection_.AddSearchStep(make_unique<Block<PlayerTest, BranchSingle>>(PlayerTest(move.player()),BranchSingle{}));
  }

  uint ConditionCase(const Condition &move) override {
    if (move.negated()) {
      return collection_.AddSearchStep(make_unique<Block<NegatedConditionCheckTest, BranchSingle>>(NegatedConditionCheckTest(
          make_unique<SearchStepsPoint>(CreateStepsInCollection(move.nfa(), collection_, declarations_))),BranchSingle{}));
    } else {
      return collection_.AddSearchStep(make_unique<Block<ConditionCheckTest, BranchSingle>>( ConditionCheckTest(
          make_unique<SearchStepsPoint>(CreateStepsInCollection(move.nfa(), collection_, declarations_))),BranchSingle{}));
    }
  }

  uint VisitedQueryCase(const VisitedQuery &move) override {
    return collection_.AddSearchStep(
        make_unique<Block<VisitedCheckTest, BranchSingle>>(VisitedCheckTest(collection_.GetBitArrayChunk(move.visited_array_index())),BranchSingle{}));

  }

  uint EmptyCase(const Empty &) override {
    return collection_.AddSearchStep(make_unique<Block<BranchMultiple>>(BranchMultiple{}));

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
    auto transitions = nfa.graph.EdgesFrom(v);
    for (const auto &transition : transitions) {
      uint u = transition.to();
      if (initialized.find(u) == initialized.end()) {
        initialized.insert(u);
        to_visit.push(u);
      }
      if (nodes_collection_indices.find(v) == nodes_collection_indices.end()) {
        nodes_collection_indices[v] = SearchStepCreator(collection, declarations)(*transition.content());
      }
    }
  }
  nodes_collection_indices[nfa.final] = collection.AddSearchStep(make_unique<Block<BranchEmpty>>(BranchEmpty()));

  to_visit = queue<node_t>();
  to_visit.push(nfa.final);
  unordered_set<node_t> visited;
  visited.insert(nfa.final);
  while (!to_visit.empty()) {
    uint v = to_visit.front();
    to_visit.pop();
    auto transitions = nfa.graph.EdgesTo(v);
    for (const auto &transition : transitions) {
      uint u = transition.from();
      if (visited.find(u) == visited.end()) {
        to_visit.push(u);
        visited.insert(u);
      }
      collection[nodes_collection_indices.at(u)]->AddNextBlock(collection[nodes_collection_indices.at(v)]);
    }
  }
  return SearchStepsPoint{collection, collection[nodes_collection_indices.at(nfa.initial)]};
}

SearchStepsInformation
rbg::CreateSearchSteps(const VisitedChecksNfa &game_graph, const Declarations &declarations) {
  SearchStepsCollection collection(game_graph.visited_checks_count, declarations.board_description.vertices_count());
  auto point = CreateStepsInCollection(game_graph.nfa, collection, declarations);
  return SearchStepsInformation{std::move(collection), point};
}