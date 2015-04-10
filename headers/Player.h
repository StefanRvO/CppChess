#pragma once
#include "Piece.h"
#include "enums.h"
#define ROOK_0 		0
#define KNIGHT_0	1
#define BISHOP_0  2
#define QUEEN_0   3
#define KING			4
#define BISHOP_1  5
#define KNIGHT_1	6
#define ROOK_1 		7


class player
{
	private:
	public:
		piece pieces[16];
    player_colour colour;
    player_type type;
    player(player_colour colour_, player_type type_);
    ~player();
};
