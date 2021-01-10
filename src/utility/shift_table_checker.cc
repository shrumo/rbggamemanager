#include "shift_table_checker.h"

#include <parser/parser_wrapper.h>

namespace rbg {
class ContainsOnlyShiftsFn : public AstFunction<bool> {
 public:
  bool DefaultCase() override { return false; }

  bool ShiftCase(const rbg_parser::shift &) override { return true; }

  bool NoopCase(const rbg_parser::noop &) override { return true; }

  bool SumCase(const rbg_parser::sum &move) override {
    for (const auto &m : move.get_content()) {
      if (!ContainsOnlyShiftsFn()(*m)) {
        return false;
      }
    }
    return true;
  }

  bool ConcatenationCase(const rbg_parser::concatenation &move) override {
    for (const auto &m : move.get_content()) {
      if (!ContainsOnlyShiftsFn()(*m)) {
        return false;
      }
    }
    return true;
  }
  bool StarCase(const rbg_parser::star_move &move) override {
    return ContainsOnlyShiftsFn()(*move.get_content());
  }
};

bool ContainsOnlyShifts(const rbg_parser::game_move &move) {
  return ContainsOnlyShiftsFn()(move);
}
}  // namespace rbg