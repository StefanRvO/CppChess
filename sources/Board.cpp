#include "../headers/Board.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
board::board(player &white_p, player &black_p,Trans_Table *t_table_ )
{
  t_table = t_table_;
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
  castling_rights.rights[0][0] = true;
  castling_rights.rights[0][1] = true;
  castling_rights.rights[1][0] = true;
  castling_rights.rights[1][1] = true;
  this->castling_rights_his.push_back(castling_rights);
  init_zob_hash();
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

  //copy castling rights

  castling_rights.rights[0][0] = old_board.castling_rights.rights[0][0];
  castling_rights.rights[0][1] = old_board.castling_rights.rights[0][1];
  castling_rights.rights[1][0] = old_board.castling_rights.rights[1][0];
  castling_rights.rights[1][1] = old_board.castling_rights.rights[1][1];
  castling_rights_his = old_board.castling_rights_his;
  //copy zob hash info
  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      for(int k = 0; k < 6; k++)
      {
        zob_table_pieces[i][j][k][0] = old_board.zob_table_pieces[i][j][k][0];
        zob_table_pieces[i][j][k][1] = old_board.zob_table_pieces[i][j][k][1];
      }
    }
  }
  zob_table_castling[0][0] = old_board.zob_table_castling[0][0];
  zob_table_castling[0][1] = old_board.zob_table_castling[0][1];
  zob_table_castling[1][0] = old_board.zob_table_castling[1][0];
  zob_table_castling[1][1] = old_board.zob_table_castling[1][1];
  zob_move = old_board.zob_move;
  zob_hash = old_board.zob_hash;
  t_table = old_board.t_table;
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

  adjust_zob_hash_unmove(moving_piece, second_piece, the_move);
  if(who2move == black) who2move = white;
  else                  who2move = black;
  this->moves.pop_back();
  this->castling_rights_his.pop_back();
  this->castling_rights = castling_rights_his.back();
}
void board::make_final_move(piece *moving_piece, move the_move)
{ //makes the move and clears the t_table. This may need to be changed.

  make_move(moving_piece, the_move);
  uint8_t move_type = the_move & MOVE_TYPE_MASK;

  switch(move_type)
  {
    case CAPTURE:
    case QUEEN_SIDE_CASTLE:
    case DOUBLEPAWN:
    case QUEENPROMO:
    case KNIGHTPROMO:
    case ROOKPROMO:
    case BISHOPPROMO:
    case KING_SIDE_CASTLE:
      t_table->clear();
    case QUIET:
      break;
  }
}

piece *board::make_move(piece *moving_piece, move the_move)
{ //assume only valide moves are given. Just obey it mindlessly
  //also adjust the zob_hash board.

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
      if(moving_piece->type == rook && moving_piece->moves == 0)
      {
        if(move_start_x == ROOK_0) castling_rights.rights[moving_piece->colour][0] = false;
        else                     castling_rights.rights[moving_piece->colour][1] = false;
      }
      else if(moving_piece->type == king && moving_piece->moves == 0)
      {
        castling_rights.rights[moving_piece->colour][0] = false;
        castling_rights.rights[moving_piece->colour][1] = false;
      }

      break;
    case CAPTURE: //set captured piece to dead, and move det capturing piece to the new field
      fields[move_end_x][move_end_y]->alive = false;
      second_piece = fields[move_end_x][move_end_y];
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      if(second_piece->type == rook && second_piece->moves == 0)
      {
        if(move_end_x == ROOK_0) castling_rights.rights[second_piece->colour][0] = false;
        else                     castling_rights.rights[second_piece->colour][1] = false;
      }
      if(moving_piece->type == rook && moving_piece->moves == 0)
      {
        if(move_start_x == ROOK_0) castling_rights.rights[moving_piece->colour][0] = false;
        else                     castling_rights.rights[moving_piece->colour][1] = false;
      }
      else if(moving_piece->type == king && moving_piece->moves == 0)
      {
        castling_rights.rights[moving_piece->colour][0] = false;
        castling_rights.rights[moving_piece->colour][1] = false;
      }
      break;
    case QUEENPROMO_CAP:  //same as capture, but with promo
      fields[move_end_x][move_end_y]->alive = false;
      second_piece = fields[move_end_x][move_end_y];
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(queen);
      if(second_piece->type == rook && second_piece->moves == 0)
      {
        if(move_end_x == ROOK_0) castling_rights.rights[second_piece->colour][0] = false;
        else                     castling_rights.rights[second_piece->colour][1] = false;
      }

      break;
    case KNIGHTPROMO_CAP: //same as capture, but with promo
      fields[move_end_x][move_end_y]->alive = false;
      second_piece = fields[move_end_x][move_end_y];
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(knight);
      if(second_piece->type == rook && second_piece->moves == 0)
      {
        if(move_end_x == ROOK_0) castling_rights.rights[second_piece->colour][0] = false;
        else                     castling_rights.rights[second_piece->colour][1] = false;
      }
      break;
    case ROOKPROMO_CAP:   //same as capture, but with promo
      fields[move_end_x][move_end_y]->alive = false;
      second_piece = fields[move_end_x][move_end_y];
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(rook);
      if(second_piece->type == rook && second_piece->moves == 0)
      {
        if(move_end_x == ROOK_0) castling_rights.rights[second_piece->colour][0] = false;
        else                     castling_rights.rights[second_piece->colour][1] = false;
      }
      break;
    case BISHOPPROMO_CAP: //same as capture, but with promo
      fields[move_end_x][move_end_y]->alive = false;
      second_piece = fields[move_end_x][move_end_y];
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      if(second_piece->type == rook && second_piece->moves == 0)
      {
        if(move_end_x == ROOK_0) castling_rights.rights[second_piece->colour][0] = false;
        else                     castling_rights.rights[second_piece->colour][1] = false;
      }
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
      castling_rights.rights[moving_piece->colour][0] = false;
      break;
    case KING_SIDE_CASTLE:
      moving_piece->move_to(move_end_x, move_end_y);
      fields[move_start_x][move_start_y] = nullptr;
      fields[move_end_x][move_end_y] = moving_piece;
      second_piece = fields[ROOK_1][move_end_y];
      fields[ROOK_1 - 2][move_end_y] = fields[ROOK_1][move_end_y];
      fields[ROOK_1][move_end_y] = nullptr;
      fields[ROOK_1 - 2][move_end_y]->x_pos = ROOK_1 - 2;
      castling_rights.rights[moving_piece->colour][0] = false;
      break;
  }
  adjust_zob_hash_move(moving_piece, second_piece, the_move);
  if(who2move == black) who2move = white;
  else                  who2move = black;
  this->moves.push_back(the_move);
  this->castling_rights_his.push_back(castling_rights);
  return second_piece;
}

void board::adjust_zob_hash_move(piece *moving_piece, piece *second_piece, move the_move)
{
  uint8_t move_end_x = (the_move & X_END_MASK) >> X_END_OFF;
  uint8_t move_end_y  = (the_move & Y_END_MASK) >> Y_END_OFF;
  uint8_t move_start_x = (the_move & X_START_MASK) >> X_START_OFF;
  uint8_t move_start_y  = (the_move & Y_START_MASK) >> Y_START_OFF;
  uint8_t move_type = the_move & MOVE_TYPE_MASK;
  if(castling_rights.rights[0][0] != castling_rights_his.back().rights[0][0]) zob_hash ^= zob_table_castling[0][0];
  if(castling_rights.rights[0][1] != castling_rights_his.back().rights[0][1]) zob_hash ^= zob_table_castling[0][1];
  if(castling_rights.rights[1][0] != castling_rights_his.back().rights[1][0]) zob_hash ^= zob_table_castling[1][0];
  if(castling_rights.rights[1][1] != castling_rights_his.back().rights[1][1]) zob_hash ^= zob_table_castling[1][0];

  switch(move_type)
  {
    case QUIET: case DOUBLEPAWN:
      zob_hash ^= zob_table_pieces[move_start_x][move_start_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[move_end_x][move_end_y][moving_piece->type][moving_piece->colour];
      break;
    case CAPTURE: //set captured piece to dead, and move det capturing piece to the new field
      zob_hash ^= zob_table_pieces[move_start_x][move_start_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[move_end_x][move_end_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[second_piece->x_pos][second_piece->y_pos][second_piece->type][second_piece->colour];
      break;
    case QUEENPROMO_CAP:  //same as capture, but with promo
    case KNIGHTPROMO_CAP: //same as capture, but with promo
    case ROOKPROMO_CAP:   //same as capture, but with promo
    case BISHOPPROMO_CAP: //same as capture, but with promo
      zob_hash ^= zob_table_pieces[move_start_x][move_start_y][pawn][moving_piece->colour];
      zob_hash ^= zob_table_pieces[move_end_x][move_end_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[second_piece->x_pos][second_piece->y_pos][second_piece->type][second_piece->colour];
      break;
    case QUEENPROMO:      //same as quiet, but with promo
    case KNIGHTPROMO:     //same as quiet, but with promo
    case ROOKPROMO:       //same as quiet, but with promo
    case BISHOPPROMO:     //same as quiet, but with promo
      break;
      zob_hash ^= zob_table_pieces[move_start_x][move_start_y][pawn][moving_piece->colour];
      zob_hash ^= zob_table_pieces[move_end_x][move_end_y][moving_piece->type][moving_piece->colour];
    case QUEEN_SIDE_CASTLE:
      zob_hash ^= zob_table_pieces[move_start_x][move_start_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[move_end_x][move_end_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[ROOK_0 ][move_start_y][rook][moving_piece->colour];
      zob_hash ^= zob_table_pieces[ROOK_0 + 3][move_start_y][rook][moving_piece->colour];
      break;
    case KING_SIDE_CASTLE:
      zob_hash ^= zob_table_pieces[move_start_x][move_start_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[move_end_x][move_end_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[ROOK_1 ][move_start_y][rook][moving_piece->colour];
      zob_hash ^= zob_table_pieces[ROOK_1 - 2][move_start_y][rook][moving_piece->colour];
      break;
  }
  zob_hash ^= zob_move;
}

void board::adjust_zob_hash_unmove(piece *moving_piece, piece *second_piece, move the_move)
{
  uint8_t move_end_x = (the_move & X_END_MASK) >> X_END_OFF;
  uint8_t move_end_y  = (the_move & Y_END_MASK) >> Y_END_OFF;
  uint8_t move_start_x = (the_move & X_START_MASK) >> X_START_OFF;
  uint8_t move_start_y  = (the_move & Y_START_MASK) >> Y_START_OFF;
  uint8_t move_type = the_move & MOVE_TYPE_MASK;

  if(castling_rights.rights[0][0] != (*(castling_rights_his.end() - 1)).rights[0][0]) zob_hash ^= zob_table_castling[0][0];
  if(castling_rights.rights[0][1] != (*(castling_rights_his.end() - 1)).rights[0][1]) zob_hash ^= zob_table_castling[0][1];
  if(castling_rights.rights[1][0] != (*(castling_rights_his.end() - 1)).rights[1][0]) zob_hash ^= zob_table_castling[1][0];
  if(castling_rights.rights[1][1] != (*(castling_rights_his.end() - 1)).rights[1][1]) zob_hash ^= zob_table_castling[1][0];

  switch(move_type)
  {
    case QUIET: case DOUBLEPAWN:
      zob_hash ^= zob_table_pieces[move_start_x][move_start_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[move_end_x][move_end_y][moving_piece->type][moving_piece->colour];
      break;
    case CAPTURE: //set captured piece to dead, and move det capturing piece to the new field
      zob_hash ^= zob_table_pieces[move_start_x][move_start_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[move_end_x][move_end_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[second_piece->x_pos][second_piece->y_pos][second_piece->type][second_piece->colour];
      break;
    case QUEENPROMO_CAP:  //same as capture, but with promo
    case KNIGHTPROMO_CAP: //same as capture, but with promo
    case ROOKPROMO_CAP:   //same as capture, but with promo
    case BISHOPPROMO_CAP: //same as capture, but with promo
      zob_hash ^= zob_table_pieces[move_start_x][move_start_y][pawn][moving_piece->colour];
      zob_hash ^= zob_table_pieces[move_end_x][move_end_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[second_piece->x_pos][second_piece->y_pos][second_piece->type][second_piece->colour];
      break;
    case QUEENPROMO:      //same as quiet, but with promo
    case KNIGHTPROMO:     //same as quiet, but with promo
    case ROOKPROMO:       //same as quiet, but with promo
    case BISHOPPROMO:     //same as quiet, but with promo
      break;
      zob_hash ^= zob_table_pieces[move_start_x][move_start_y][pawn][moving_piece->colour];
      zob_hash ^= zob_table_pieces[move_end_x][move_end_y][moving_piece->type][moving_piece->colour];
    case QUEEN_SIDE_CASTLE:
      zob_hash ^= zob_table_pieces[move_start_x][move_start_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[move_end_x][move_end_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[ROOK_0 ][move_start_y][rook][moving_piece->colour];
      zob_hash ^= zob_table_pieces[ROOK_0 + 3][move_start_y][rook][moving_piece->colour];
      break;
    case KING_SIDE_CASTLE:
      zob_hash ^= zob_table_pieces[move_start_x][move_start_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[move_end_x][move_end_y][moving_piece->type][moving_piece->colour];
      zob_hash ^= zob_table_pieces[ROOK_1 ][move_start_y][rook][moving_piece->colour];
      zob_hash ^= zob_table_pieces[ROOK_1 - 2][move_start_y][rook][moving_piece->colour];
      break;
  }
  zob_hash ^= zob_move;
}


void board::init_zob_hash()
{
  //seed
  srand(time(NULL));
  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      for(int k = 0; k < 6; k++)
      {
        zob_table_pieces[i][j][k][0] = uint64_t(rand()%(1 << 16)) << 48 |  uint64_t(rand()%(1 << 16)) << 32 |  uint64_t(rand()%(1 << 16)) << 16 |  uint64_t(rand()%(1 << 16));
        zob_table_pieces[i][j][k][1] = uint64_t(rand()%(1 << 16)) << 48 |  uint64_t(rand()%(1 << 16)) << 32 |  uint64_t(rand()%(1 << 16)) << 16 |  uint64_t(rand()%(1 << 16));
      }
    }
  }
  zob_table_castling[0][0] = uint64_t(rand()%(1 << 16)) << 48 |  uint64_t(rand()%(1 << 16)) << 32 |  uint64_t(rand()%(1 << 16)) << 16 |  uint64_t(rand()%(1 << 16));
  zob_table_castling[0][1] = uint64_t(rand()%(1 << 16)) << 48 |  uint64_t(rand()%(1 << 16)) << 32 |  uint64_t(rand()%(1 << 16)) << 16 |  uint64_t(rand()%(1 << 16));
  zob_table_castling[1][0] = uint64_t(rand()%(1 << 16)) << 48 |  uint64_t(rand()%(1 << 16)) << 32 |  uint64_t(rand()%(1 << 16)) << 16 |  uint64_t(rand()%(1 << 16));
  zob_table_castling[1][1] = uint64_t(rand()%(1 << 16)) << 48 |  uint64_t(rand()%(1 << 16)) << 32 |  uint64_t(rand()%(1 << 16)) << 16 |  uint64_t(rand()%(1 << 16));
  zob_move = uint64_t(rand()%(1 << 16)) << 48 |  uint64_t(rand()%(1 << 16)) << 32 |  uint64_t(rand()%(1 << 16)) << 16 |  uint64_t(rand()%(1 << 16));
  zob_hash = uint64_t(rand()%(1 << 16)) << 48 |  uint64_t(rand()%(1 << 16)) << 32 |  uint64_t(rand()%(1 << 16)) << 16 |  uint64_t(rand()%(1 << 16));
}
