//
// Created by shrum on 23.06.19.
//

#include <queue>
#include <game_state/blocks/actions.h>
#include "blocks_creator.h"
#include "game_description/construction/arithmetic_creator.h"

using namespace rbg;
using namespace std;

SearchStepsPoint
CreateStepsInCollection(const Nfa<std::unique_ptr<Move>> &game_graph, BlocksCollection &collection,
                        const Declarations &declarations);

template<typename Branch>
struct BlocksCreatorResult {
  uint block_collection_index;
  Branch *branch_pointer;
};

template<typename Branch>
class SearchStepCreator : public MoveFunction<BlocksCreatorResult<Branch>> {
public:
  explicit SearchStepCreator(BlocksCollection &collection, const Declarations &declarations,
                             bool register_modifiers = true)
      : collection_(collection), declarations_(declarations), register_modifiers_(register_modifiers) {
  }

  BlocksCreatorResult<Branch> ShiftCase(const Shift &move) override {
    auto block = CreateBlockUniquePtr(ShiftAction(move.edge_id()), OutOfBoundsTest{}, Branch{});
    auto branch = block->content().branch();
    uint index = collection_.AddBlock(std::move(block));
    return BlocksCreatorResult<Branch>{index, branch};
  }

  BlocksCreatorResult<Branch> ShiftTableCase(const ShiftTable &) override {
    assert(false && "not yet implemented");
  }

  BlocksCreatorResult<Branch> ArithmeticComparisonCase(const ArithmeticComparison &move) override {
    auto left_result = CreateArithmeticOperation(move.left());
    auto right_result = CreateArithmeticOperation(move.right());
    switch (move.comparison_type()) {
      case ComparisonType::kEqual: {
        auto block = CreateBlockUniquePtr(EqualComparisonTest(
            std::move(left_result), std::move(right_result)), Branch{});
        auto branch = block->content().branch();
        uint index = collection_.AddBlock(std::move(block));
        return {index, branch};
      }
      case ComparisonType::kNotEqual: {
        auto block = CreateBlockUniquePtr(NotEqualComparisonTest(
                std::move(left_result),
                std::move(right_result)
            ), Branch{});
        auto branch = block->content().branch();
        uint index = collection_.AddBlock(std::move(block));
        return {index, branch};
      }
      case ComparisonType::kLess: {
         auto block = CreateBlockUniquePtr(LessComparisonTest(
                std::move(left_result),
                std::move(right_result)
            ), Branch{});
        auto branch = block->content().branch();
        uint index = collection_.AddBlock(std::move(block));
        return {index, branch};
      }
      case ComparisonType::kLessEqual: {
         auto block = CreateBlockUniquePtr(LessEqualComparisonTest(
                std::move(left_result),
                std::move(right_result)
            ), Branch{});
        auto branch = block->content().branch();
        uint index = collection_.AddBlock(std::move(block));
        return {index, branch};
      }

      case ComparisonType::kGreater: {
         auto block = CreateBlockUniquePtr(LessComparisonTest(
                std::move(right_result),
                std::move(left_result)
            ), Branch{});
        auto branch = block->content().branch();
        uint index = collection_.AddBlock(std::move(block));
        return {index, branch};
      }
      case ComparisonType::kGreaterEqual: {
         auto block = CreateBlockUniquePtr(LessEqualComparisonTest(
                std::move(right_result),
                std::move(left_result)
            ), Branch{});
        auto branch = block->content().branch();
        uint index = collection_.AddBlock(std::move(block));
        return {index, branch};
      }
    }
    assert(false && "Not possible.");
  }

  BlocksCreatorResult<Branch> OffCase(const Off &move) override {
     auto block = CreateBlockUniquePtr(OffApplication(move.piece(), move.index()), Branch{});
     auto branch = block->content().branch();

    if (register_modifiers_) {
      collection_.RegisterModifier(move.index(),block->template GetAction<0>());
    }
    uint step_index = collection_.AddBlock(std::move(block));
    return {step_index, branch};
  }

  BlocksCreatorResult<Branch> OnCase(const On &move) override {
    auto block = CreateBlockUniquePtr(OnTest(move.pieces_table()), Branch{});
    auto branch = block->content().branch();
    uint step_index = collection_.AddBlock(std::move(block));
    return {step_index, branch};
  }

  BlocksCreatorResult<Branch> PlayerSwitchCase(const PlayerSwitch &move) override {
    auto block = CreateBlockUniquePtr(PlayerSwitchApplication(move.player(), move.index()), Branch{});
    auto branch = block->content().branch();
    if (register_modifiers_) {
      collection_.RegisterModifier(move.index(),block->template GetAction<0>());

    }
      collection_.RegisterSwitch(move.index(), block->template GetSubAbstractBlock<1>());
    uint step_index = collection_.AddBlock(std::move(block));
    return {step_index, branch};
  }

  BlocksCreatorResult<Branch> KeeperSwitchCase(const KeeperSwitch &move) override {
     auto block = CreateBlockUniquePtr(PlayerSwitchApplication(move.keeper_id(), move.index()), Branch{});
    auto branch = block->content().branch();
    if (register_modifiers_) {
      collection_.RegisterModifier(move.index(),block->template GetAction<0>());

    }
      collection_.RegisterSwitch(move.index(), block->template GetSubAbstractBlock<1>());
    uint step_index = collection_.AddBlock(std::move(block));
    return {step_index, branch};
  }

  BlocksCreatorResult<Branch> AssignmentCase(const Assignment &move) override {
    auto block = CreateBlockUniquePtr(AssignmentAction(move.get_variable(),
                                                                                 CreateArithmeticOperation(
                                                                                     move.get_value_expression()),
                                                                                 move.index()),
                                                                VariableBoundsTest(move.get_variable()),
                                                                Branch{});
    auto branch = block->content().branch();
    if (register_modifiers_) {
      collection_.RegisterModifier(move.index(), block->template GetAction<0>());
    }
    uint step_index = collection_.AddBlock(std::move(block));
    return {step_index, branch};
  }

  BlocksCreatorResult<Branch> PlayerCheckCase(const PlayerCheck &move) override {
    auto block = CreateBlockUniquePtr(PlayerTest(move.player()), Branch{});
    auto branch = block->content().branch();
    uint step_index = collection_.AddBlock(std::move(block));
    return {step_index, branch};
  }

  BlocksCreatorResult<Branch> ConditionCase(const Condition &move) override {
    if (move.negated()) {
      auto block = CreateBlockUniquePtr(NegatedConditionCheckTest(
          CreateStepsInCollection(move.nfa(), collection_, declarations_).current()), Branch{});
      auto branch = block->content().branch();
      uint step_index = collection_.AddBlock(std::move(block));
      return {step_index, branch};
    } else {
      auto block = CreateBlockUniquePtr(ConditionCheckTest(
          CreateStepsInCollection(move.nfa(), collection_, declarations_).current()), Branch{});
      auto branch = block->content().branch();
      uint step_index = collection_.AddBlock(std::move(block));
      return {step_index, branch};
    }
  }

  BlocksCreatorResult<Branch> VisitedQueryCase(const VisitedQuery &move) override {

    auto block = CreateBlockUniquePtr(VisitedCheckTest(collection_.GetBitArrayChunk(move.visited_array_index())),
                             Branch{});
    auto branch = block->content().branch();
    uint step_index = collection_.AddBlock(std::move(block));
    return {step_index, branch};
  }

private:
  BlocksCollection &collection_;
  const Declarations &declarations_;
  bool register_modifiers_;
};

BlocksCreatorResult<BranchMultiple> CreateMultipleBranch(BlocksCollection *collection) {
  auto block = CreateBlockUniquePtr(BranchMultiple{});
    auto branch = block->content().branch();
    uint step_index = collection->AddBlock(std::move(block));
    return {step_index, branch};
}



void CreateStepsInCollectionRecursive(const Graph<std::unique_ptr<Move>> &game_graph, BlocksCollection &collection,
                                      const Declarations &declarations, node_t current,
                                      unordered_map<node_t, uint> *nodes_blocks) {
  if(game_graph.EdgesFrom(current).size() == 0) {
    (*nodes_blocks)[current] = collection.AddBlock(CreateBlockUniquePtr(BranchEmpty()));
  }
  else if(game_graph.EdgesFrom(current).size() == 1) {
    const auto& edge = *game_graph.EdgesFrom(current).begin();
    uint next = edge.to();
    auto result = SearchStepCreator<BranchSingle>(collection, declarations)(*edge.content());
    (*nodes_blocks)[current]  = result.block_collection_index;
    if(nodes_blocks->find(next) == nodes_blocks->end() ) {
      CreateStepsInCollectionRecursive(game_graph, collection, declarations, next, nodes_blocks);
    }
    result.branch_pointer->AddNext(collection.GetBlockPointer(nodes_blocks->at(next)));
  }
  else {
    auto result = CreateMultipleBranch(&collection);
    (*nodes_blocks)[current]  = result.block_collection_index;

    for(const auto& edge : game_graph.EdgesFrom(current)) {
      assert(edge.content()->type() == MoveType::kEmpty && "The multiple branch now only support only epsilon transitions.");
       uint next = edge.to();
       if(nodes_blocks->find(next) == nodes_blocks->end() ) {
         CreateStepsInCollectionRecursive(game_graph, collection, declarations, next, nodes_blocks);
       }
       result.branch_pointer->AddNext(collection.GetBlockPointer(nodes_blocks->at(next)));
    }
  }
}

SearchStepsPoint
CreateStepsInCollection(const Nfa<std::unique_ptr<Move>> &game_graph, BlocksCollection &collection,
                        const Declarations &declarations) {
  const auto &nfa = game_graph;
  std::unordered_map<node_t, uint> nodes_blocks;
  CreateStepsInCollectionRecursive(nfa.graph, collection, declarations, nfa.initial, &nodes_blocks);
  return SearchStepsPoint{collection, collection.GetBlockPointer(nodes_blocks.at(nfa.initial))};
}

SearchStepsInformation
rbg::CreateSearchSteps(const VisitedChecksNfa &game_graph, const Declarations &declarations) {
  BlocksCollection collection(game_graph.visited_checks_count, declarations.initial_board().vertices_count());
  auto point = CreateStepsInCollection(game_graph.nfa, collection, declarations);
  return SearchStepsInformation{std::move(collection), point};
}