//
// Created by shrum on 28.05.19.
//

#include "test.h"
//
// Created by shrum on 12.01.18.
//

#include <array>
#include <iostream>

#include <fstream>
#include <sstream>
#include <chrono>

#include <unordered_map>
#include <memory>
#include <limits>
#include <string>
#include <vector>

#include <rbgParser/src/game_items.hpp>

#include "parser/parser_connection.h"

#include "boost/program_options.hpp"

#include "game_components/game_description.h"
#include "game_components/game_state.h"


const char *kAmazons = R"DELIMITER(
// Amazons

#players = white(100), black(100)
#pieces = e, w, b, x
#variables =
#board = rectangle(up,down,left,right,
[e, e, e, b, e, e, b, e, e, e]
[e, e, e, e, e, e, e, e, e, e]
[e, e, e, e, e, e, e, e, e, e]
[b, e, e, e, e, e, e, e, e, b]
[e, e, e, e, e, e, e, e, e, e]
[e, e, e, e, e, e, e, e, e, e]
[w, e, e, e, e, e, e, e, e, w]
[e, e, e, e, e, e, e, e, e, e]
[e, e, e, e, e, e, e, e, e, e]
[e, e, e, w, e, e, w, e, e, e])

#anySquare = ((up* + down*)(left* + right*))

#queenShift = (
(up left {e}) (up left {e})* +
(up {e}) (up {e})* +
(up right {e}) (up right {e})* +
(left {e}) (left {e})* +
(right {e}) (right {e})* +
(down left {e}) (down left {e})* +
(down {e}) (down {e})* +
(down right {e}) (down right {e})*
)

#turn(piece; player) = (
anySquare {piece} [e]
queenShift
[piece]
queenShift
->> [x]
)

#rules = ->white (
turn(w; white) [$ white=100, black=0] ->black
    turn(b; black) [$ white=0, black=100] ->white
)*
)DELIMITER";


const char *kChess = R"DELIMITER(
// Chess
// -- No rules about repeating states or tie after given number of turns without pawn moves.
// -- The play is limited to 200 turns.

#anyRook(color) = color~RookMoved, color~RookUnmoved
#majorPieces(color) = color~RookMoved, color~Knight, color~Bishop, color~Queen
#singleColorPieces(color) = color~Pawn, color~King, anyRook(color), color~Knight, color~Bishop, color~Queen
#emptyOrColor(color) = singleColorPieces(color), empty
#allPieces = singleColorPieces(white), singleColorPieces(black)

#players = white(100), black(100)
#pieces = allPieces, empty
#variables = pawnDoubleMove(1), whiteKingMoved(1), blackKingMoved(1), turnCount(200)

#board = rectangle(up,down,left,right,
         [blackRookUnmoved, blackKnight, blackBishop, blackQueen, blackKing, blackBishop, blackKnight, blackRookUnmoved]
         [blackPawn       , blackPawn  , blackPawn  , blackPawn , blackPawn, blackPawn  , blackPawn  , blackPawn       ]
         [empty           , empty      , empty      , empty     , empty    , empty      ,empty       , empty           ]
         [empty           , empty      , empty      , empty     , empty    , empty      ,empty       , empty           ]
         [empty           , empty      , empty      , empty     , empty    , empty      ,empty       , empty           ]
         [empty           , empty      , empty      , empty     , empty    , empty      ,empty       , empty           ]
         [whitePawn       , whitePawn  , whitePawn  , whitePawn , whitePawn, whitePawn  , whitePawn  , whitePawn       ]
         [whiteRookUnmoved, whiteKnight, whiteBishop, whiteQueen, whiteKing, whiteBishop, whiteKnight, whiteRookUnmoved])

#anySquare = (left* + right*)(up* + down*)
#anyNeighborSquare = (left + right + up + down + (left + right)(up + down))

#diagonalLine = (
        (up left {empty})* up left
      + (up right {empty})* up right
      + (down left {empty})* down left
      + (down right {empty})* down right
    )

#parallelLine = (
        (up {empty})* up
      + (down {empty})* down
      + (right {empty})* right
      + (left {empty})* left
    )

#knightHop = (
        (up^2 + down^2)(left + right)
      + (left^2 + right^2)(up + down)
    )

#captureAnyBy(capturingPiece; oppColor) = {emptyOrColor(oppColor)} [capturingPiece]

#pickUpPiece(piece) = {piece}[empty]

#standardMove(piece; movePattern; oppColor) =
    pickUpPiece(piece)
    movePattern
    captureAnyBy(piece; oppColor)

#bishopMove(color; oppColor) = standardMove(color~Bishop; diagonalLine; oppColor)
#knightMove(color; oppColor) = standardMove(color~Knight; knightHop; oppColor)
#queenMove(color; oppColor) = standardMove(color~Queen; (diagonalLine + parallelLine); oppColor)

#rookMove(color; oppColor) =
    pickUpPiece(anyRook(color))
    parallelLine
    captureAnyBy(color~RookMoved; oppColor)

#isAttackedBy(oppColor; forward; backward) = (
        knightHop {oppColor~Knight}
      + forward left ({oppColor~King,oppColor~Pawn} + ({empty} forward left)* {oppColor~Queen,oppColor~Bishop})
      + forward right ({oppColor~King,oppColor~Pawn} + ({empty} forward right)* {oppColor~Queen,oppColor~Bishop})
      + backward left ({oppColor~King} + ({empty} backward left)* {oppColor~Queen,oppColor~Bishop})
      + backward right ({oppColor~King} + ({empty} backward right)* {oppColor~Queen,oppColor~Bishop})
      + up ({oppColor~King} + ({empty} up)* {oppColor~Queen,anyRook(oppColor)})
      + down ({oppColor~King} + ({empty} down)* {oppColor~Queen,anyRook(oppColor)})
      + left ({oppColor~King} + ({empty} left)* {oppColor~Queen,anyRook(oppColor)})
      + right ({oppColor~King} + ({empty} right)* {oppColor~Queen,anyRook(oppColor)})
    )

#checked(color; oppColor; forward; backward) =
    anySquare {color~King} isAttackedBy(oppColor; forward; backward)

#standardKingMove(color; oppColor) =
    anyNeighborSquare
    captureAnyBy(color~King; oppColor)

#castlingKingMove(forward; backward; color; oppColor) =
    {$ color~KingMoved==0}
    {! isAttackedBy(oppColor; forward; backward)}
    (
        right {empty} {! isAttackedBy(oppColor; forward; backward)}
        right {empty} [color~King]
        right pickUpPiece(color~RookUnmoved)
        left^2
      + left {empty} {! isAttackedBy(oppColor; forward; backward)}
        left {empty} [color~King]
        left {empty} // Rook CAN pass through an attacked square
        left pickUpPiece(color~RookUnmoved)
        right^3
    )
    [color~RookMoved]

#kingMove(forward; backward; color; oppColor) = (
      pickUpPiece(color~King)
      (
          standardKingMove(color; oppColor)
        + castlingKingMove(forward; backward; color; oppColor)
      ) [$ color~KingMoved=1]
    )

#checkForPromotion(forward; color) = (
        {? forward} [color~Pawn]
      + {! forward} [majorPieces(color)]
    )

#standardPawnMove(forward; backward; color; oppColor) =
    pickUpPiece(color~Pawn)
    (
        (
            forward {empty}
          + forward (left + right) {singleColorPieces(oppColor)}
        )
        checkForPromotion(forward; color)
      + {! backward^2}
        (forward {empty})^2
        [color~Pawn] // Promotion cannot occur
        [$ pawnDoubleMove=1]
    )

#enPassantPawnMove(forward; color) =
    {$ pawnDoubleMove==1}
    (
        // We are sure that thd current square is occupied by oppColor~Pawn, because pawnDoubleMove==1, so do not check it
        right pickUpPiece(color~Pawn) left
      + left pickUpPiece(color~Pawn) right
    )
    [empty] forward [color~Pawn] [$ pawnDoubleMove=0]

#move(forward; backward; color; oppColor) = (
     (
          [$ pawnDoubleMove=0]
          anySquare
          (
              kingMove(forward; backward; color; oppColor)
            + bishopMove(color; oppColor)
            + rookMove(color; oppColor)
            + queenMove(color; oppColor)
            + knightMove(color; oppColor)
            + standardPawnMove(forward; backward; color; oppColor)
          )
        + enPassantPawnMove(forward; color)
     )  {! checked(color; oppColor; forward; backward)}
  )

#turn(forward; backward; color; oppColor) = (
    move(forward; backward; color; oppColor)
    ->>
    [$ turnCount = turnCount+1]
    (
        {$ turnCount < 200} ->> (
            {! checked(oppColor; color; backward; forward)} [$ color=50,oppColor=50] // Draw if stalemate (no further legal move)
          + {? checked(oppColor; color; backward; forward)} [$ color=100,oppColor=0] // Win if checkmate (no further legal move)
        ) ->oppColor
      + {$ turnCount == 200} [$ color=50,oppColor=50] ->> (
            {! checked(oppColor; color; backward; forward)} ->> {} // Draw if not checked
          + {? checked(oppColor; color; backward; forward)}
            {! move(forward; backward; color; oppColor)}
            [$ color=100,oppColor=0] ->> {} // Win if checkmate
        )
    )
  )

#rules = ->white (
        turn(up;down;white;black)
        turn(down;up;black;white)
    )*
)DELIMITER";

const char *kHex = R"DELIMITER(
// Hex

#players = red(100), blue(100)
#pieces = e, r, b
#variables =
#board = hexagon(NW, NE, E, SE, SW, W,
                      [ e ]
                    [ e , e ]
                  [ e , e , e ]
                [ e , e , e , e ]
              [ e , e , e , e , e ]
            [ e , e , e , e , e , e ]
          [ e , e , e , e , e , e , e ]
        [ e , e , e , e , e , e , e , e ]
      [ e , e , e , e , e , e , e , e , e ]
    [ e , e , e , e , e , e , e , e , e , e ]
  [ e , e , e , e , e , e , e , e , e , e , e ]
    [ e , e , e , e , e , e , e , e , e , e ]
      [ e , e , e , e , e , e , e , e , e ]
        [ e , e , e , e , e , e , e , e ]
          [ e , e , e , e , e , e , e ]
            [ e , e , e , e , e , e ]
              [ e , e , e , e , e ]
                [ e , e , e , e ]
                  [ e , e , e ]
                    [ e , e ]
                      [ e ])

#anyNeighbor = (NW + NE + E + SE + SW + W)
#anySquare = anyNeighbor*

#reachable(dir; piece) = ((anyNeighbor {piece})* {! dir})

#turn(player; oppPlayer; dir1; dir2; piece) =
  anySquare {e} ->> [piece]
  (
     ({! reachable(dir1; piece)} + {! reachable(dir2; piece)})
     ->oppPlayer
    + {? reachable(dir1; piece)} {? reachable(dir2; piece)}
      [$ player=100, oppPlayer=0]
      ->> {}
  )

#rules = ->red (
    turn(red; blue; NW; SE; r)
    turn(blue; red; NE; SW; b)
  )*
)DELIMITER";

using uint = unsigned int;

extern uint kTextWidth;

PerftResult perft(SearchContext *context, GameState *state, size_t depth) {
  size_t next_perft = state->player() == state->description().keeper_player_id() ? depth : depth - 1;

  if (next_perft <= 0) {
    return {1, 1};
  }

  std::vector<Move> moves;
  if (state->player() ==
      state->description().keeper_player_id()) {
    moves = state->FindFirstMove(context);
  } else {
    moves = state->FindMoves(context);
  }
  size_t leaf_count = 0;
  auto node_count = static_cast<size_t>(state->player() != state->description().keeper_player_id());
  for (const auto &move : moves) {
    auto revert_info = state->MakeRevertibleMove(move);
    auto rec_res = perft(context, state, next_perft);
    leaf_count += rec_res.leaf_count;
    node_count += rec_res.node_count;
    state->RevertMove(revert_info);
  }
  return {leaf_count, node_count};
}

PerftResult perft_test(const std::string &game, size_t depth) {
  rbg_parser::messages_container msg;
  std::unique_ptr<rbg_parser::parsed_game> pg;
  std::vector<rbg_parser::token> result = tokenize(game, msg);
  rbg_parser::game_items g = input_tokens(result, msg);

  pg = std::unique_ptr<rbg_parser::parsed_game>(
      new rbg_parser::parsed_game(g.parse_game(msg)));
  ASSERT(msg.is_empty())
  GameDescription gd = CreateDescription(*pg);
  SearchContext context;
  GameState state(gd);
  return perft(&context, &state, depth + 1);
}

int main() {
  ASSERT(perft_test(kAmazons, 1).leaf_count == 2176)
  ASSERT(perft_test(kHex, 2).leaf_count == 14520)
  ASSERT(perft_test(kChess, 3).leaf_count == 8902)
}
