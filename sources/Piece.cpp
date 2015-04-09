#include "../headers/Piece.h"

piece::piece(uint8_t x, uint8_t y, piece_type type_, player_colour colour_)
{
  this->x_pos = x;
  this->y_pos = y;
  this->moves = 0;
  this->colour = colour_;
  change_type(type_);
  this->alive = true;
}
piece::piece()
{

}
piece::~piece()
{

}

void piece::move_to(uint8_t x, uint8_t y)
{
  this->x_pos = x;
  this->y_pos = y;
  this->moves++;
}

void piece::move_back_to(uint8_t x, uint8_t y)
{
  this->x_pos = x;
  this->y_pos = y;
  this->moves--;
}

void piece::change_type(piece_type newtype)
{
  if(this->type == newtype) return;

  this->type  = newtype;
  switch(this->type)
  {
    case pawn:
      this->value = PAWNVAL;
      break;
    case knight:
      this->value = KNIGHTVAL;
      break;
    case bishop:
      this->value = BISHOPVAL;
      break;
    case rook:
      this->value = ROOKVAL;
      break;
    case queen:
      this->value = QUEENVAL;
      break;
    case king:
      this->value = KINGVAL;
      break;
  }
  return;
}
