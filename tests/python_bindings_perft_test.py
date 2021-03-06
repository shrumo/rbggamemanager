#! /usr/bin/python3
from rbg import *

chess = '''
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
'''

def perft(state, depth):
  if depth == 0:
    return [1,1]
  moves = state.Moves()
  result = [0,0]
  result[1] += 1
  for move in moves:
    revert_info = state.Apply(move)
    rec_result = perft(state, depth-1)
    result[0] += rec_result[0]
    result[1] += rec_result[1]
    state.Revert(revert_info)
  return result

game_state = CreateGameState(chess)
result = perft(game_state, 3)
print("Perft for chess:",result)
assert(result[0] == 8902)
assert(result[1] == 9323)
