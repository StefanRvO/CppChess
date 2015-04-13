#include "../headers/Board.h"
#include <iostream>
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

board::board(player &white_p, player &black_p, board &old_board)
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
  this->moves = old_board.moves;
  this->who2move = old_board.who2move;
}


board::~board()
{

}
bool board::is_board_consistent()
{
  for(uint8_t i = 0; i < 8; i++)
  {
    for(uint8_t j = 0; j < 8; j++)
    {
      if(fields[i][j] == nullptr) continue;
      if(fields[i][j]->x_pos != i || fields[i][j]->y_pos != j || fields[i][j]->alive == false)
      {
        std::cout << int(i) << " " << int(j) << std::endl;
        std::cout << fields[i][j]->alive << std::endl;
        return false;

      }
    }
  }
  return true;
}

std::string board::get_board_string()
{
  std::string boardstr;
  for(uint8_t i = 0; i <= 7; i++)
  {
      for(uint8_t j = 0; j <= 7; j++)
      {
        if(fields[j][i] != nullptr && fields[j][i]->alive)
        {
          switch(fields[j][i]->type)
          {
            case pawn:
              boardstr += "P"; break;
            case bishop:
              boardstr += "R"; break;
            case king:
              boardstr += "K"; break;
            case queen:
              boardstr += "Q"; break;
            case rook:
              boardstr += "T"; break;
            case knight:
              boardstr += "H"; break;
          }
      }
      else boardstr += " ";
      boardstr += "|";
    }
    boardstr += "\n";
  }
  return boardstr;
}

void board::print_board()
{
  std::cout << get_board_string();
}

void board::unmake_move(piece *moving_piece, move the_move, piece *second_piece)
{  //unmakes a move. a second piece can be giving if the move affected two pieces (captures, castling)
  //the moves which affects two pieces are castling and captures.

  uint8_t move_end_x = (the_move & X_END_MASK) >> X_END_OFF;
  uint8_t move_end_y  = (the_move & Y_END_MASK) >> Y_END_OFF;
  uint8_t move_start_x = (the_move & X_START_MASK) >> X_START_OFF;
  uint8_t move_start_y  = (the_move & Y_START_MASK) >> Y_START_OFF;

  uint8_t move_type = the_move & MOVE_TYPE_MASK;

  switch(move_type)
  {
    case CAPTURE:
      moving_piece->move_back_to(move_start_x, move_start_y);
      fields[move_start_x][move_start_y] = moving_piece;
      fields[move_end_x][move_end_y] = second_piece;
      second_piece->alive = true;
      break;
    case QUEENPROMO_CAP:
    case KNIGHTPROMO_CAP:
    case ROOKPROMO_CAP:
    case BISHOPPROMO_CAP:
      moving_piece->move_back_to(move_start_x, move_start_y);
      fields[move_start_x][move_start_y] = moving_piece;
      fields[move_end_x][move_end_y] = second_piece;
      second_piece->alive = true;
      moving_piece->change_type(pawn);
      break;
    case QUEEN_SIDE_CASTLE:
      moving_piece->move_back_to(move_start_x, move_start_y);
      fields[move_start_x][move_start_y] = moving_piece;
      fields[move_end_x][move_end_y] = nullptr;
      fields[second_piece->x_pos][move_end_y] = nullptr;
      fields[ROOK_0][move_end_y] = second_piece;
      second_piece->x_pos = ROOK_0;
      break;
    case KING_SIDE_CASTLE:
      moving_piece->move_back_to(move_start_x, move_start_y);
      fields[move_start_x][move_start_y] = moving_piece;
      fields[move_end_x][move_end_y] = nullptr;
      fields[second_piece->x_pos][move_end_y] = nullptr;
      fields[ROOK_1][move_end_y] = second_piece;
      second_piece->x_pos = ROOK_1;
      break;
    case QUIET:
    case DOUBLEPAWN:
      moving_piece->move_back_to(move_start_x, move_start_y);
      fields[move_start_x][move_start_y] = moving_piece;
      fields[move_end_x][move_end_y] = nullptr;
      break;
    case QUEENPROMO:
    case KNIGHTPROMO:
    case ROOKPROMO:
    case BISHOPPROMO:
      moving_piece->move_back_to(move_start_x, move_start_y);
      fields[move_start_x][move_start_y] = moving_piece;
      fields[move_end_x][move_end_y] = nullptr;
      moving_piece->change_type(pawn);
      break;
    default:
      std::cout << "Error in unmake move" << std::endl;
      break;
  }
  if(who2move == black) who2move = white;
  else                  who2move = black;
  this->moves.pop_back();
}

piece *board::make_move(piece *moving_piece, move the_move)
{ //assume only valide moves are given. Just obey it mindlessly

  uint8_t move_end_x = (the_move & X_END_MASK) >> X_END_OFF;
  uint8_t move_end_y  = (the_move & Y_END_MASK) >> Y_END_OFF;
  uint8_t move_start_x = (the_move & X_START_MASK) >> X_START_OFF;
  uint8_t move_start_y  = (the_move & Y_START_MASK) >> Y_START_OFF;
  piece *second_piece = nullptr;
  uint8_t move_type = the_move & MOVE_TYPE_MASK;
  switch(move_type)
  {
    case QUIET: case DOUBLEPAWN:
      //Just move the piece and update game_board
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      break;
    case CAPTURE: //set captured piece to dead, and move det capturing piece to the new field
      fields[move_end_x][move_end_y]->alive = false;
      second_piece = fields[move_end_x][move_end_y];
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      break;
    case QUEENPROMO_CAP:  //same as capture, but with promo
      fields[move_end_x][move_end_y]->alive = false;
      second_piece = fields[move_end_x][move_end_y];
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(queen);
      break;
    case KNIGHTPROMO_CAP: //same as capture, but with promo
      fields[move_end_x][move_end_y]->alive = false;
      second_piece = fields[move_end_x][move_end_y];
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(knight);
      break;
    case ROOKPROMO_CAP:   //same as capture, but with promo
      fields[move_end_x][move_end_y]->alive = false;
      second_piece = fields[move_end_x][move_end_y];
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(rook);
      break;
    case BISHOPPROMO_CAP: //same as capture, but with promo
      fields[move_end_x][move_end_y]->alive = false;
      second_piece = fields[move_end_x][move_end_y];
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(bishop);
      break;
    case QUEENPROMO:      //same as quiet, but with promo
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(queen);
      break;
    case KNIGHTPROMO:     //same as quiet, but with promo
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(knight);
      break;
    case ROOKPROMO:       //same as quiet, but with promo
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(rook);
      break;
    case BISHOPPROMO:     //same as quiet, but with promo
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(bishop);
      break;
    case QUEEN_SIDE_CASTLE:
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      second_piece = fields[ROOK_0][move_end_y];
      fields[ROOK_0 + 3][move_end_y] = fields[ROOK_0][move_end_y];
      fields[ROOK_0][move_end_y] = nullptr;
      fields[ROOK_0 + 3][move_end_y]->x_pos = ROOK_0 + 3;
      break;
    case KING_SIDE_CASTLE:
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      second_piece = fields[ROOK_1][move_end_y];
      fields[ROOK_1 - 2][move_end_y] = fields[ROOK_1][move_end_y];
      fields[ROOK_1][move_end_y] = nullptr;
      fields[ROOK_1 - 2][move_end_y]->x_pos = ROOK_1 - 2;
      break;
  }

  if(who2move == black) who2move = white;
  else                  who2move = black;
  this->moves.push_back(the_move);
  return second_piece;
}
