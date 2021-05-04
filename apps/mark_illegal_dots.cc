// We define an illegal dot as a dot in the rules that would increase the number
// of moves if it was treated as a modifier. This app will move each such dot
// in given rules to the next (to the right) possible legal space.  An example
// of a space that is always legal is the one next to a modifier.
#include <game_state/construction/game_state_creator.h>
#include <stl_extension/argparse.h>
#include <utility/calculate_perft.h>
#include <utility/printer.h>
#include <utility/shift_table_checker.h>

#include <cassert>
#include <chrono>
#include <fstream>
#include <queue>
#include <random>

using namespace rbg;
using namespace std;

// This is the dfa state used for the FindIllegalNoopEdgesRec function.
struct DfaState {
  unordered_set<node_t> finished;
  unordered_map<node_t, int> visited_in_times;
  unordered_map<node_t,
                shared_ptr<vector<pair<int, /*board_product*/ edge_id_t>>>>
      visited_dot_applications;
  shared_ptr<vector<pair<int, /*board_product*/ edge_id_t>>>
      current_dots_applications =
          make_shared<vector<pair<int, /*board_product*/ edge_id_t>>>();
  int current_time = 0;

  unordered_set<edge_id_t> result;
};

// TODO: Take into account modifiers
// Two cases to think about:
// A loop that jumps back behind a modifier
// A + with a modifier in one of the branches and not the other

// TODO: Document what this does and how it does it

Declarations *glob_declarations;

void FindIllegalNoopEdgesRec(const NfaBoardProduct &board_product,
                             /*board_product*/ node_t current_node,
                             DfaState &dfa_state) {
  if (dfa_state.visited_in_times.find(current_node) !=
      dfa_state.visited_in_times.end()) {
    return;
  }

  dfa_state.visited_in_times[current_node] = dfa_state.current_time;
  dfa_state.current_time++;
  dfa_state.visited_dot_applications[current_node] =
      dfa_state.current_dots_applications;

  for (const Edge</*nfa*/ edge_id_t> &edge :
       board_product.EdgesFrom(current_node)) {

    shared_ptr<vector<pair<int, /*board_product*/ edge_id_t>>>
        previous_dots_applications = dfa_state.current_dots_applications;

    if (board_product.original_nfa()
            ->graph.GetEdge(edge.content())
            .content()
            ->type() == MoveType::kNoop) {
      dfa_state.current_dots_applications =
          make_shared<vector<pair<int, /*board_product*/ edge_id_t>>>(
              dfa_state.current_dots_applications->begin(),
              dfa_state.current_dots_applications->end());
      dfa_state.current_dots_applications->push_back(
          {dfa_state.visited_in_times[current_node], edge.id()});
    }

    if (dfa_state.visited_in_times.find(edge.to()) !=
        dfa_state.visited_in_times.end()) {
      // Loop case
      if (dfa_state.finished.find(edge.to()) == dfa_state.finished.end()) {
        for (const pair<int, /*board_product*/ edge_id_t> &dot_application :
             *dfa_state.current_dots_applications) {
          int in_time = dfa_state.visited_in_times.at(edge.to());
          if (dot_application.first > in_time) {
            // The dot is in a loop, mark it as illegal
            dfa_state.result.insert(
                board_product.GetEdge(dot_application.second).content());
          }
        }
        goto cleanup;
      }

      shared_ptr<vector<pair<int, /*board_product*/ edge_id_t>>>
          &other_dots_applications =
              dfa_state.visited_dot_applications.at(edge.to());

      if (other_dots_applications != dfa_state.current_dots_applications) {
        for (const pair<int, /*board_product*/ edge_id_t> &dot_application :
             *other_dots_applications) {
          dfa_state.result.insert(
              board_product.GetEdge(dot_application.second).content());
        }
        for (const pair<int, /*board_product*/ edge_id_t> dot_application :
             *dfa_state.current_dots_applications) {
          dfa_state.result.insert(
              board_product.GetEdge(dot_application.second).content());
        }
        goto cleanup;
      }
    }

    if (!IsModifier(board_product.original_nfa()
                        ->graph.GetEdge(edge.content())
                        .content()
                        ->type())) {
      FindIllegalNoopEdgesRec(board_product, edge.to(), dfa_state);
    }

  cleanup:
    dfa_state.current_dots_applications = previous_dots_applications;
  }
  dfa_state.finished.insert(current_node);
}

unordered_set<const rbg_parser::game_move *>
FindIllegalNoops(const Nfa<std::unique_ptr<Move>> &nfa,
                 const Declarations &declarations) {

  NfaBoardProduct board_product(nfa, declarations.initial_board(),
                                /*initials=*/{/*vertex_id=*/1});

  unordered_set<const rbg_parser::game_move *> result;
  unordered_set<node_t> explored_nodes;

  // Explore starting from the initial node explictly
  node_t initial_board_product_node =
      board_product.node(/*vertex=*/1, nfa.initial);

  DfaState dfa_state;
  FindIllegalNoopEdgesRec(board_product, initial_board_product_node, dfa_state);

  for (edge_id_t edge : dfa_state.result) {
    result.insert(nfa.graph.GetEdge(edge).content()->original_move());
  }

  explored_nodes.insert(initial_board_product_node);

  // Explore starting from each modifier (as the dfs stops at modifiers)
  for (/*board_product*/ node_t node : board_product.nodes()) {
    for (const Edge<edge_id_t> &edge : board_product.EdgesTo(node)) {
      if (IsModifier(board_product.original_nfa()
                         ->graph.GetEdge(edge.content())
                         .content()
                         ->type())) {
        if (explored_nodes.find(node) != explored_nodes.end()) {
          continue;
        }
        DfaState dfa_state;
        FindIllegalNoopEdgesRec(board_product, node, dfa_state);

        for (edge_id_t edge : dfa_state.result) {
          result.insert(nfa.graph.GetEdge(edge).content()->original_move());
        }
        explored_nodes.insert(node);
      }
    }
  }

  return result;
}

const char *kIndentChars = "  ";

std::string times(const std::string &x, size_t n) {
  std::string result;
  for (size_t i = 0; i < n; i++) {
    result += x;
  }
  return result;
}

std::string trim(std::string s) {
  s.erase(s.find_last_not_of(" \n\r\t") + 1);
  s.erase(0, s.find_first_not_of(" \n\r\t"));
  return s;
}

class DotsMarkerPrinter : public AstFunction<std::string> {
public:
  DotsMarkerPrinter(
      const unordered_set<const rbg_parser::game_move *> &dots_to_mark,
      int indent = 0)
      : dots_to_mark_(dots_to_mark), indent_(indent) {}
  std::string SumCase(const rbg_parser::sum &move) override {
    if (move.get_content().size() == 1) {
      return DotsMarkerPrinter(dots_to_mark_,
                               indent_)(*move.get_content().front());
    }

    std::string result;

    result += "(";
    result += "\n" + times(kIndentChars, indent_ + 1);

    bool first = true;
    for (const auto &m : move.get_content()) {
      if (first) {
        first = false;
      } else {
        result += "+";
        result += "\n" + times(kIndentChars, indent_ + 1);
      }

      result += DotsMarkerPrinter(dots_to_mark_, indent_ + 1)(*m);
    }

    result += "\n" + times(kIndentChars, indent_);
    result += ")";

    return result;
  }

  std::string
  ConcatenationCase(const rbg_parser::concatenation &move) override {
    if (move.get_content().size() == 1) {
      return DotsMarkerPrinter(dots_to_mark_,
                               indent_)(*move.get_content().front());
    }

    std::string result;

    result += "( ";

    bool first = true;

    for (size_t i = 0; i < move.get_content().size(); i++) {
      const auto &m = move.get_content()[i];
      if (first) {
        first = false;
      } else {
        result += " ";
      }
      result += DotsMarkerPrinter(dots_to_mark_, indent_)(*m);
    }

    result += " )";

    return result;
  }
  std::string StarCase(const rbg_parser::star_move &move) override {
    std::string result;
    result += DotsMarkerPrinter(dots_to_mark_, indent_)(*move.get_content());
    result += "*";
    return result;
  }

  std::string MoveCheckCase(const rbg_parser::move_check &move) override {
    std::string prefix = move.is_negated() ? "{!" : "{?";
    return prefix + "\n" + times(kIndentChars, indent_ + 1) +
           DotsMarkerPrinter(dots_to_mark_, indent_ + 1)(*move.get_content()) +
           "\n" + times(kIndentChars, indent_) + "}";
  }

  std::string ArithmeticExpressionCase(
      const rbg_parser::arithmetic_expression &expr) override {
    return expr.to_rbg();
  }

  std::string GameMoveCase(const rbg_parser::game_move &move) override {
    return trim(move.to_rbg(rbg_parser::options{}));
  }

  std::string NoopCase(const rbg_parser::noop &move) override {
    if (dots_to_mark_.find(&move) != dots_to_mark_.end()) {
      return "/*illegal*/.";
    }
    return ".";
  }

private:
  const std::unordered_set<const rbg_parser::game_move *> dots_to_mark_;
  size_t indent_;
};

int main(int argc, const char *argv[]) {
  auto args = std_ext::parse_args(argc, argv);

  if (args.positional_args.size() != 1) {
    std::cout << "Usage: " << argv[0] << " <game_file>" << std::endl;
    return 0;
  }

  std::ifstream file_stream(args.positional_args[0]);
  std::stringstream buffer;
  buffer << file_stream.rdbuf();

  std::unique_ptr<rbg_parser::parsed_game> parsed_game;
  try {
    parsed_game = ParseGame(buffer.str());
  } catch (rbg_parser::message m) {
    std::cout << m.as_warning() << std::endl;
  }

  Declarations declarations = Declarations(*parsed_game);
  glob_declarations = &declarations;
  auto nfa =
      CreateNfa(*parsed_game->get_moves(), declarations, /*optimize=*/false);
  unordered_set<const rbg_parser::game_move *> illegal_noop_moves =
      FindIllegalNoops(nfa, declarations);

  std::stringstream result;

  result << "#board = " << parsed_game->get_board().to_rbg(true) << "\n";
  result << parsed_game->get_declarations().to_rbg() << "\n";
  result << "#rules = "
         << DotsMarkerPrinter(illegal_noop_moves)(*parsed_game->get_moves())
         << std::endl;

  std::cout << result.str();
}