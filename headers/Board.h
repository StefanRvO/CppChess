#pragma once
#include "Player.h"
#include "Piece.h"
#include "Move.h"
#include <string>
class board
{
  private:
  public:
    piece *fields[8][8];
    player_colour who2move = white;
    board(player &white_p, player &black_p);
    ~board();
    bool is_board_consistent();
    void print_board();
    std::string get_board_string();
    piece *make_move(piece *moving_piece, move the_move);
    void unmake_move(piece *moving_piece, move the_move, piece *second_piece);



};
