#pragma once
#include "Piece.h"
#include "enums.h"

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
