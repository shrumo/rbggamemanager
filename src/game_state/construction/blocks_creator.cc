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
CreateStepsInCollection(const Nfa<std::unique_ptr<Move>> &nfa, BlocksCollection &collection,
                        const Declarations &declarations, const std::unordered_map<edge_id_t, uint> &visited_check_indices);

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
    assert(false && "This type of comparison is not supported.");
    return {};
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

private:
  BlocksCollection &collection_;
  const Declarations &declarations_;
  bool register_modifiers_;
};

template<typename Branch>
class VisitedCheckCreator : public MoveFunction<BlocksCreatorResult<Branch>> {
public:
  explicit VisitedCheckCreator(BlocksCollection &collection, const Declarations &declarations,
                             uint visited_check_index)
      : collection_(collection), declarations_(declarations), visited_check_index_(visited_check_index) {
  }

  BlocksCreatorResult<Branch> VisitedQueryCase(const VisitedQuery &) override {
    auto block = CreateBlockUniquePtr(VisitedCheckTest(collection_.GetBitArrayChunk(visited_check_index_)),
                             Branch{});
    auto branch = block->content().branch();
    uint step_index = collection_.AddBlock(std::move(block));
    return {step_index, branch};
  }

  private:
  BlocksCollection &collection_;
  const Declarations &declarations_;
  uint visited_check_index_;
};

  template<typename Branch>
class ConditionCheckCreator : public MoveFunction<BlocksCreatorResult<Branch>> {
public:
  explicit ConditionCheckCreator(BlocksCollection &collection, const Declarations &declarations,
                             const Graph<std::unique_ptr<Move>> &graph,
                              const std::unordered_map<edge_id_t, uint> &visited_check_indices)
      : collection_(collection), declarations_(declarations), graph_(graph), visited_check_indices_(visited_check_indices) {
  }


   BlocksCreatorResult<Branch> ConditionCase(const Condition &move) override {
    if (move.negated()) {
      auto block = CreateBlockUniquePtr(NegatedConditionCheckTest(
          CreateStepsInCollection(move.nfa(), collection_, declarations_, visited_check_indices_).current()), Branch{});
      auto branch = block->content().branch();
      uint step_index = collection_.AddBlock(std::move(block));
      return {step_index, branch};
    } else {
      auto block = CreateBlockUniquePtr(ConditionCheckTest(
          CreateStepsInCollection(move.nfa(), collection_, declarations_, visited_check_indices_).current()), Branch{});
      auto branch = block->content().branch();
      uint step_index = collection_.AddBlock(std::move(block));
      return {step_index, branch};
    }
  }

private:
  BlocksCollection &collection_;
  const Declarations &declarations_;
  const Graph<std::unique_ptr<Move>> &graph_;
  const std::unordered_map<edge_id_t, uint> &visited_check_indices_;
};

BlocksCreatorResult<BranchMultiple> CreateMultipleBranch(BlocksCollection *collection) {
  auto block = CreateBlockUniquePtr(BranchMultiple{});
    auto branch = block->content().branch();
    uint step_index = collection->AddBlock(std::move(block));
    return {step_index, branch};
}

void VisitedCheckIndicesRecursive(const Graph<std::unique_ptr<Move>>& game_graph,
    node_t current,
    std::unordered_map<edge_id_t ,uint> *result,
    std::unordered_set<node_t> *visited) {
  visited->insert(current);
  for(const auto& edge : game_graph.EdgesFrom(current)) {
    uint next = edge.to();
    if(edge.content()->type() == MoveType :: kVisitedCheck) {
      uint index = result->size();
      (*result)[edge.id()] = index;
    }
    if(edge.content()->type() == MoveType :: kConditionCheck) {
      const auto& condition = dynamic_cast<const Condition&>(*edge.content());
      std::unordered_set<node_t> sub_graph_visited;
      VisitedCheckIndicesRecursive(condition.nfa().graph, condition.nfa().initial, result, &sub_graph_visited);
    }
    if(visited->find(next) == visited->end()) {
      VisitedCheckIndicesRecursive(game_graph, next, result, visited);
    }
  }
}

std::unordered_map<edge_id_t, uint> VisitedCheckIndices(const Nfa<std::unique_ptr<Move>> &nfa) {
  std::unordered_map<edge_id_t, uint> result;
  std::unordered_set<node_t> visited;
  VisitedCheckIndicesRecursive(nfa.graph, nfa.initial, &result, &visited);
  return result;
}

void CreateStepsInCollectionRecursive(const Graph<std::unique_ptr<Move>> &game_graph, BlocksCollection &collection,
                                      const Declarations &declarations,
                                      const std::unordered_map<edge_id_t, uint>& visited_check_indices,
                                      node_t current,
                                      unordered_map<node_t, uint> *nodes_blocks) {

  if(game_graph.EdgesFrom(current).size() == 0) {
    (*nodes_blocks)[current] = collection.AddBlock(CreateBlockUniquePtr(BranchEmpty()));
  }
  else if(game_graph.EdgesFrom(current).size() == 1) {
    const auto& edge = *game_graph.EdgesFrom(current).begin();
    uint next = edge.to();
    BlocksCreatorResult<BranchSingle> result{};
    if(edge.content()->type() == MoveType :: kVisitedCheck) {
      uint visited_check_index = visited_check_indices.at(edge.id());
      result = VisitedCheckCreator<BranchSingle>(collection, declarations, visited_check_index)(*edge.content());
    } else if(edge.content()->type() == MoveType :: kConditionCheck) {
      result = ConditionCheckCreator<BranchSingle>(collection, declarations, game_graph, visited_check_indices)(*edge.content());
    } else {
      result = SearchStepCreator<BranchSingle>(collection, declarations)(*edge.content());
    }
    (*nodes_blocks)[current]  = result.block_collection_index;
    if(nodes_blocks->find(next) == nodes_blocks->end() ) {
      CreateStepsInCollectionRecursive(game_graph, collection, declarations, visited_check_indices, next, nodes_blocks);
    }
    result.branch_pointer->AddNext(collection.GetBlockPointer(nodes_blocks->at(next)));
  }
  else {
    auto result = CreateMultipleBranch(&collection);
    (*nodes_blocks)[current]  = result.block_collection_index;

    for(const auto& edge : game_graph.EdgesFrom(current)) {
      assert(edge.content()->type() == MoveType::kEmpty && "The multiple branch only supports epsilon transitions for now.");
       uint next = edge.to();
       if(nodes_blocks->find(next) == nodes_blocks->end() ) {
         CreateStepsInCollectionRecursive(game_graph, collection, declarations, visited_check_indices, next, nodes_blocks);
       }
       result.branch_pointer->AddNext(collection.GetBlockPointer(nodes_blocks->at(next)));
    }
  }
}

SearchStepsPoint
CreateStepsInCollection(const Nfa<std::unique_ptr<Move>>& nfa, BlocksCollection &collection,
                        const Declarations &declarations, const std::unordered_map<edge_id_t, uint>& visited_check_indices) {
  std::unordered_map<node_t, uint> nodes_blocks;
  CreateStepsInCollectionRecursive(nfa.graph, collection, declarations, visited_check_indices, nfa.initial , &nodes_blocks);
  return SearchStepsPoint{collection, collection.GetBlockPointer(nodes_blocks.at(nfa.initial))};
}

SearchStepsInformation
rbg::CreateSearchSteps(const Nfa<std::unique_ptr<Move>> &game_nfa, const Declarations &declarations) {
  auto visited_indices = VisitedCheckIndices(game_nfa);
  BlocksCollection collection(visited_indices.size(), declarations.initial_board().vertices_count());
  auto point = CreateStepsInCollection(game_nfa, collection, declarations, visited_indices);
  return SearchStepsInformation{std::move(collection), point};
}