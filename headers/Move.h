#pragma once
//Class for holding information about a move
//The variable for the move is this:
//Defines for movetypes
#define QUIET                 0
#define CAPTURE               1
#define DOUBLEPAWN            2
#define QUEEN_SIDE_CASTLE     3
#define KING_SIDE_CASTLE      4
#define QUEENPROMO            5
#define KNIGHTPROMO           6
#define ROOKPROMO             7
#define BISHOPPROMO           8
#define QUEENPROMO_CAP        9
#define KNIGHTPROMO_CAP       10
#define ROOKPROMO_CAP         11
#define BISHOPPROMO_CAP       12

typedef uint16_t move;

struct move_score
{
  move the_move;
  int32_t score;
};

#define X_START_OFF 13
#define X_END_OFF   10
#define Y_START_OFF 7
#define Y_END_OFF   4
#define X_START_MASK   0b1110000000000000
#define X_END_MASK     0b1110000000000
#define Y_START_MASK   0b1110000000
#define Y_END_MASK     0b1110000
#define MOVE_TYPE_MASK 0b1111

static bool  move_score_comp(move_score mv1, move_score mv2);
static bool __attribute__((unused))  move_score_comp(move_score mv1, move_score mv2)
{
  return mv1.score > mv2.score;
}

//|startpos_x|endpos_x|startpos_y|endpos_y|move_type|
//|     3    |    3   |    3     |   3    |    4    |
