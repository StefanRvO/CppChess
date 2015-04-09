#pragma once
#include "Player.h"
#include "Piece.h"
class board
{
  private:
  public:
    piece *fields[8][8];
    player_colour who2move = white;
    board(player &white_p, player &black_p);
    ~board();
};
 
