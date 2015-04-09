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
  pieces[0] = piece(0, starty, rook,   this->colour);
  pieces[1] = piece(1, starty, knight, this->colour);
  pieces[2] = piece(2, starty, bishop, this->colour);
  pieces[3] = piece(3, starty, queen,  this->colour);
  pieces[4] = piece(4, starty, king,   this->colour);
  pieces[5] = piece(5, starty, bishop, this->colour);
  pieces[6] = piece(6, starty, knight, this->colour);
  pieces[7] = piece(7, starty, rook,   this->colour);
  for(uint8_t i = 0; i <= 7; i++)
  {
    pieces[i + 8] = piece(i, nexty, pawn, this->colour);
  }
}
player::~player()
{

}
