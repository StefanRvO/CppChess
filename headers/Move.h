#pragma once
//Class for holding information about a move
//The variable for the move is this:
//Defines for movetypes
#define QUIET       0
#define DOUBLEPAWN  1
#define RIGHTCASTLE 2
#define LEFTCASTLE  3
#define QUEENPROMO  4
#define KNIGHTPROMO 5
#define ROOKPROMO   6
#define BISHOPPROMO 7
typedef uint16_t move;

#define X_START_OFF 13
#define X_END_OFF   10
#define Y_START_OFF 7
#define Y_END_OFF   4
//|startpos_x|endpos_x|startpos_y|endpos_y|move_type|
//|     3    |    3   |    3     |   3    |    4    |
