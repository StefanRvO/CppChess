#include "../headers/Board.h"
board::board(player &white_p, player &black_p)
{
  for(uint8_t i = 0; i < 8; i++)
  {
    for(uint8_t j = 0; j < 8; j++)
    {
      this->fields[i][j] = nullptr;
    }
  }
  for(uint8_t i = 0; i < 16; i++)
  {
    if(white_p.pieces[i].alive)
      this->fields[white_p.pieces[i].x_pos][white_p.pieces[i].y_pos] = &(white_p.pieces[i]);
    if(black_p.pieces[i].alive)
      this->fields[black_p.pieces[i].x_pos][black_p.pieces[i].y_pos] = &(black_p.pieces[i]);
  }
}

board::~board()
{

}
