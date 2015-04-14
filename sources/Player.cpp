#include "../headers/Player.h"

player::player(player_colour colour_, player_type type_)
{
  this->colour = colour_;
  this->type = type_;
  uint8_t starty = 0;
  uint8_t nexty = 1;

  if(this->colour == black)
  {
    starty = 7;
    nexty  = 6;
  }
  pieces[ROOK_0]   = piece(ROOK_0,   starty, rook,   this->colour);
  pieces[KNIGHT_0] = piece(KNIGHT_0, starty, knight, this->colour);
  pieces[BISHOP_0] = piece(BISHOP_0, starty, bishop, this->colour);
  pieces[QUEEN_0]  = piece(QUEEN_0,  starty, queen,  this->colour);
  pieces[KING]     = piece(KING,     starty, king,   this->colour);
  pieces[BISHOP_1] = piece(BISHOP_1, starty, bishop, this->colour);
  pieces[KNIGHT_1] = piece(KNIGHT_1, starty, knight, this->colour);
  pieces[ROOK_1]   = piece(ROOK_1,   starty, rook,   this->colour);

  for(uint8_t i = 0; i <= 7; i++)
  {
    pieces[i + 8] = piece(i, nexty, pawn, this->colour);
  }
}
player::~player()
{

}
