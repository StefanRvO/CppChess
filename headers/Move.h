#pragma once
//Class for holding information about a move
//The variable for the move is this:
//Defines for movetypes
#define QUIET           0
#define CAPTURE         1
#define DOUBLEPAWN      2
#define RIGHTCASTLE     3
#define LEFTCASTLE      4
#define QUEENPROMO      5
#define KNIGHTPROMO     6
#define ROOKPROMO       7
#define BISHOPPROMO     8
#define CAPTURE         1
#define QUEENPROMO_CAP  9
#define KNIGHTPROMO_CAP 10
#define ROOKPROMO_CAP   11
#define BISHOPPROMO_CAP 12

typedef uint16_t move;

#define X_START_OFF 13
#define X_END_OFF   10
#define Y_START_OFF 7
#define Y_END_OFF   4
//|startpos_x|endpos_x|startpos_y|endpos_y|move_type|
//|     3    |    3   |    3     |   3    |    4    |
