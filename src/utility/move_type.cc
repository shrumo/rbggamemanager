#include <utility/move_type.h>

#include "parser/parser_wrapper.h"

namespace rbg {


class GetNodeTypeFn : public AstFunction<NodeType> {
  public:
     NodeType SumCase(const rbg_parser::sum &move) override { return NodeType::kSum; }
     NodeType ConcatenationCase(const rbg_parser::concatenation &move) override { return NodeType::kConcat; }
     NodeType StarCase(const rbg_parser::star_move &move) override { return NodeType::kStar; }
     NodeType GameMoveCase(const rbg_parser::game_move &) { return NodeType::kMove; }
};

NodeType ParserNodeType(const rbg_parser::game_move & move) {
    return GetNodeTypeFn()(move);
}

}