//
// Created by shrum on 23.06.19.
//

#include <queue>
#include <game_state/search_steps/actions.h>
#include "blocks_creator.h"
#include "game_description/construction/arithmetic_creator.h"

using namespace rbg;
using namespace std;

SearchStepsPoint
CreateStepsInCollection(const Nfa<std::unique_ptr<Move>> &game_graph, BlocksCollection &collection,
                        const Declarations &declarations);

template<typename Branch>
class SearchStepCreator : public MoveFunction<uint> {
public:
  explicit SearchStepCreator(BlocksCollection &collection, const Declarations &declarations,
                             bool register_modifiers = true)
      : collection_(collection), declarations_(declarations), register_modifiers_(register_modifiers) {
  }

  uint ShiftCase(const Shift &move) override {
    uint index = collection_.AddBlock(
        CreateBlockUniquePtr(ShiftAction(move.edge_id()), OutOfBoundsTest{}, Branch{}));
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
        return collection_.AddBlock(CreateBlockUniquePtr(EqualComparisonTest(
            std::move(left_result), std::move(right_result)), Branch{}));
      case ComparisonType::kNotEqual:
        return collection_.AddBlock(
            CreateBlockUniquePtr(NotEqualComparisonTest(
                std::move(left_result),
                std::move(right_result)
            ), Branch{}));
      case ComparisonType::kLess:
        return collection_.AddBlock(CreateBlockUniquePtr(LessComparisonTest(
            std::move(left_result),
            std::move(right_result)
        ), Branch{}));
      case ComparisonType::kLessEqual:
        return collection_.AddBlock(
            CreateBlockUniquePtr(LessEqualComparisonTest(
                std::move(left_result),
                std::move(right_result)), Branch{}));
      case ComparisonType::kGreater:
        return collection_.AddBlock(CreateBlockUniquePtr(LessComparisonTest(
            std::move(right_result),
            std::move(left_result)
        ), Branch{}));
      case ComparisonType::kGreaterEqual:
        return collection_.AddBlock(
            CreateBlockUniquePtr(LessEqualComparisonTest(
                std::move(right_result),
                std::move(left_result)
            ), Branch{}));
    }
    assert(false && "Not possible.");
    return 0;
  }

  uint OffCase(const Off &move) override {
    uint step_index = collection_.AddBlock(
        CreateBlockUniquePtr(OffApplication(move.piece(), move.index()), Branch{}));
    if (register_modifiers_) {
      collection_.RegisterModifier(move.index(),
                                   static_cast<ActionsBlock<OffApplication, Branch> *>(collection_.GetBlockPointer(step_index))->template GetAction<0>());
    }
    return step_index;
  }

  uint OnCase(const On &move) override {
    return collection_.AddBlock(CreateBlockUniquePtr(OnTest(move.pieces_table()), Branch{}));

  }

  uint PlayerSwitchCase(const PlayerSwitch &move) override {
    uint step_index = collection_.AddBlock(
        CreateBlockUniquePtr(PlayerSwitchApplication(move.player(), move.index()), Branch{}));
    if (register_modifiers_) {
      collection_.RegisterModifier(move.index(),
                                   dynamic_cast<ActionsBlock<PlayerSwitchApplication, Branch> *>(collection_.GetBlockPointer(step_index))->template GetAction<0>());

    }

      collection_.RegisterSwitch(move.index(), dynamic_cast<ActionsBlock<PlayerSwitchApplication, Branch>*>(collection_.GetBlockPointer(step_index))->template GetSubAbstractBlock<1>());
    return step_index;
  }

  uint KeeperSwitchCase(const KeeperSwitch &move) override {
     uint step_index = collection_.AddBlock(
         CreateBlockUniquePtr(PlayerSwitchApplication(move.keeper_id(), move.index()), Branch{}));
    if (register_modifiers_) {

      collection_.RegisterModifier(move.index(),
                                   dynamic_cast<ActionsBlock<PlayerSwitchApplication, Branch> *>(collection_.GetBlockPointer(step_index))->template GetAction<0>());
    }


    collection_.RegisterSwitch(move.index(), dynamic_cast<ActionsBlock<PlayerSwitchApplication, Branch>*>(collection_.GetBlockPointer(step_index))->template GetSubAbstractBlock<1>());
    return step_index;
  }

  uint AssignmentCase(const Assignment &move) override {
    uint step_index = collection_.AddBlock(CreateBlockUniquePtr(AssignmentAction(move.get_variable(),
                                                                                 CreateArithmeticOperation(
                                                                                     move.get_value_expression()),
                                                                                 move.index()),
                                                                VariableBoundsTest(move.get_variable()),
                                                                Branch{}));
    if (register_modifiers_) {
      auto tmp = dynamic_cast<ActionsBlock<AssignmentAction, VariableBoundsTest, Branch> *>(collection_.GetBlockPointer(step_index));
      collection_.RegisterModifier(move.index(),
                                   tmp->template GetAction<0>());
    }
    return step_index;
  }

  uint PlayerCheckCase(const PlayerCheck &move) override {
    return collection_.AddBlock(CreateBlockUniquePtr(PlayerTest(move.player()), Branch{}));
  }

  uint ConditionCase(const Condition &move) override {
    if (move.negated()) {
      return collection_.AddBlock(CreateBlockUniquePtr(NegatedConditionCheckTest(
          CreateStepsInCollection(move.nfa(), collection_, declarations_).current()), Branch{}));
    } else {
      return collection_.AddBlock(CreateBlockUniquePtr(ConditionCheckTest(
          CreateStepsInCollection(move.nfa(), collection_, declarations_).current()), Branch{}));
    }
  }

  uint VisitedQueryCase(const VisitedQuery &move) override {
    return collection_.AddBlock(
        CreateBlockUniquePtr(VisitedCheckTest(collection_.GetBitArrayChunk(move.visited_array_index())),
                             Branch{}));

  }

  uint EmptyCase(const Empty &) override {
    return collection_.AddBlock(CreateBlockUniquePtr(Branch{}));

  }

private:
  BlocksCollection &collection_;
  const Declarations &declarations_;
  bool register_modifiers_;
};



void CreateStepsInCollectionDfs(const Graph<std::unique_ptr<Move>> &game_graph, BlocksCollection &collection,
                        const Declarations &declarations, node_t current,
                        unordered_set<node_t> *visited,
                        unordered_map<node_t, uint> *nodes_blocks) {
  visited->insert(current);
  if(game_graph.EdgesFrom(current).size() == 0) {
    (*nodes_blocks)[current] = collection.AddBlock(CreateBlockUniquePtr(BranchEmpty()));
  }
  for(const auto& edge : game_graph.EdgesFrom(current)) {
    uint next = edge.to();
    if(nodes_blocks->find(current) == nodes_blocks->end()) {
      if(game_graph.EdgesFrom(current).size() == 1) {
        (*nodes_blocks)[current] = SearchStepCreator<BranchSingle>(collection, declarations)(*edge.content());
      } else {
        (*nodes_blocks)[current] = SearchStepCreator<BranchMultiple>(collection, declarations)(*edge.content());
      }
    }
    if(visited->find(next) == visited->end() ) {
      CreateStepsInCollectionDfs(game_graph, collection, declarations, next, visited, nodes_blocks);
    }
    collection.GetBlockPointer(nodes_blocks->at(current))->AddNextBlock(collection.GetBlockPointer(nodes_blocks->at(next)));
  }
}

SearchStepsPoint
CreateStepsInCollection(const Nfa<std::unique_ptr<Move>> &game_graph, BlocksCollection &collection,
                        const Declarations &declarations) {
  const auto &nfa = game_graph;
  std::unordered_set<node_t> visited;
  std::unordered_map<node_t, uint> nodes_blocks;
  CreateStepsInCollectionDfs(nfa.graph, collection, declarations,nfa.initial, &visited, &nodes_blocks);
  return SearchStepsPoint{collection, collection.GetBlockPointer(nodes_blocks.at(nfa.initial))};
}

SearchStepsInformation
rbg::CreateSearchSteps(const VisitedChecksNfa &game_graph, const Declarations &declarations) {
  BlocksCollection collection(game_graph.visited_checks_count, declarations.initial_board.vertices_count());
  auto point = CreateStepsInCollection(game_graph.nfa, collection, declarations);
  return SearchStepsInformation{std::move(collection), point};
}