//
// Created by shrum on 15.06.19.
//

//
// Created by shrum on 03.06.19.
//

//
// Created by shrum on 03.06.19.
//
#include <parser/parser_wrapper.h>
#include <cassert>
#include <construction/graph_creator.h>
#include <construction/searchsteps_creator.h>
#include <queue>

using namespace rbg;
using namespace std;

const char *kSmallGame = R"LIM(
#players = red(100), blue(100)
#pieces = e
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [e,e]
    [e,e])
//       1       2    3      4      5        6                 7
#rules = ->red ( up + down + left + right )* [$ turn = turn+1] ->blue
)LIM";

int main() {
  auto pg = ParseGame(kSmallGame);
  auto graph = CreateGraph(*pg->get_moves());
  auto res = CreateSearchSteps(graph);
  assert(res.first != nullptr);
}