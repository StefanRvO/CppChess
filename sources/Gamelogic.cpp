#include "../headers/Gamelogic.h"
#include <cstdlib>

static void find_pseudo_legal_pawn_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_knight_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_rook_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_bishop_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_queen_moves(piece *this_piece, board *game_board, std::vector<move> *moves);
/*
static void find_pseudo_legal_king_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

*/
void find_legal_moves(piece *this_piece, player __attribute__((unused)) *this_player, player __attribute__((unused)) *opponent, board *game_board, std::vector<move> *moves)
{
  find_pseudo_legal_moves(this_piece, game_board, moves);
}
/* This function generates all moves
** without considering if the moves
** will make it's own king chess. */
void find_pseudo_legal_moves(piece *this_piece, board *game_board, std::vector<move> *moves)
{
  switch(this_piece->type)
  {
  	case pawn:
      find_pseudo_legal_pawn_moves(this_piece, game_board, moves);
      break;
  	case knight:
      find_pseudo_legal_knight_moves(this_piece, game_board, moves);
      break;
  	case rook:
      find_pseudo_legal_rook_moves(this_piece, game_board, moves);
      break;
    case bishop:
      find_pseudo_legal_bishop_moves(this_piece, game_board, moves);
      break;
  	case queen:
      find_pseudo_legal_queen_moves(this_piece, game_board, moves);
  	  break;/*
  	case king:
      find_pseudo_legal_king_moves(this_piece, game_board, moves);
      break;*/
    default:
      break;
  }
}

/* This function generates all moves for a pawn
** without considering if the moves
** will make it's own king chess. */
static void find_pseudo_legal_pawn_moves(piece *this_piece, board *game_board, std::vector<move> *moves)
{
  if(this_piece->colour == white) //Find for white pawns
  {
    if(this_piece->moves == 0) //first move
    {
      if(game_board->fields[this_piece->x_pos][this_piece->y_pos + 2] == nullptr &&
          game_board->fields[this_piece->x_pos][this_piece->y_pos + 1] == nullptr) //don't need boundchecking here
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 2) << Y_END_OFF |
              DOUBLEPAWN );
    }
    if(this_piece->y_pos == 6) // check for promotions
    {
      if(game_board->fields[this_piece->x_pos][this_piece->y_pos + 1] == nullptr) //don't need boundchecking here.
      {
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
              QUEENPROMO );
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
              KNIGHTPROMO );
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
              ROOKPROMO );
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
              BISHOPPROMO );
      }
      if(this_piece->x_pos != 0) //check capture left
      {
        if(game_board->fields[this_piece->x_pos - 1][this_piece->y_pos + 1] != nullptr &&
          game_board->fields[this_piece->x_pos - 1][this_piece->y_pos + 1]->colour == black)
        {
        moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos - 1) << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
              QUEENPROMO_CAP );
        moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos - 1) << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
              KNIGHTPROMO_CAP );
        moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos - 1) << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
              ROOKPROMO_CAP );
        moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos - 1) << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
              BISHOPPROMO_CAP );
        }
      }
        if(this_piece->x_pos != 7) //check capture right
        {
          if(game_board->fields[this_piece->x_pos + 1][this_piece->y_pos + 1] != nullptr &&
            game_board->fields[this_piece->x_pos + 1][this_piece->y_pos + 1]->colour == black)
          {
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos + 1) << X_END_OFF |
                this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
                QUEENPROMO_CAP );
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos + 1) << X_END_OFF |
                this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
                KNIGHTPROMO_CAP );
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos + 1) << X_END_OFF |
                this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
                ROOKPROMO_CAP );
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos + 1) << X_END_OFF |
                this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
                BISHOPPROMO_CAP );
          }
        }
    }
    else
    {
      if(game_board->fields[this_piece->x_pos][this_piece->y_pos + 1] == nullptr) //don't need boundchecking here.
      {
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
              QUIET );
      }
      if(this_piece->x_pos != 0) //left capture
      {
        if(game_board->fields[this_piece->x_pos - 1][this_piece->y_pos + 1] != nullptr &&
         game_board->fields[this_piece->x_pos - 1][this_piece->y_pos + 1]-> colour == black)
        {
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos - 1) << X_END_OFF |
                this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
                CAPTURE );
        }
      }
      if(this_piece->x_pos != 7) //right capture
      {
        if(game_board->fields[this_piece->x_pos + 1][this_piece->y_pos + 1] != nullptr &&
         game_board->fields[this_piece->x_pos + 1][this_piece->y_pos + 1]-> colour == black)
        {
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos + 1) << X_END_OFF |
                this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 1) << Y_END_OFF |
                CAPTURE );
        }
      }
    }
  }



  else //Find for black pawns
  {
    if(this_piece->moves == 0) //first move
    {
      if(game_board->fields[this_piece->x_pos][this_piece->y_pos - 2] == nullptr &&
          game_board->fields[this_piece->x_pos][this_piece->y_pos - 1] == nullptr) //don't need boundchecking here
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 2) << Y_END_OFF |
              DOUBLEPAWN );
    }
    if(this_piece->y_pos == 1) // check for promotions
    {
      if(game_board->fields[this_piece->x_pos][this_piece->y_pos - 1] == nullptr) //don't need boundchecking here.
      {
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
              QUEENPROMO );
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
              KNIGHTPROMO );
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
              ROOKPROMO );
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
              BISHOPPROMO );
      }
      if(this_piece->x_pos != 0) //check capture left
      {
        if(game_board->fields[this_piece->x_pos - 1][this_piece->y_pos - 1] != nullptr &&
          game_board->fields[this_piece->x_pos - 1][this_piece->y_pos - 1]->colour == white)
        {
        moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos - 1) << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
              QUEENPROMO_CAP );
        moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos - 1) << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
              KNIGHTPROMO_CAP );
        moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos - 1) << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
              ROOKPROMO_CAP );
        moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos - 1) << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
              BISHOPPROMO_CAP );
        }
      }
        if(this_piece->x_pos != 7) //check capture right
        {
          if(game_board->fields[this_piece->x_pos + 1][this_piece->y_pos - 1] != nullptr &&
            game_board->fields[this_piece->x_pos + 1][this_piece->y_pos - 1]->colour == white)
          {
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos + 1) << X_END_OFF |
                this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
                QUEENPROMO_CAP );
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos + 1) << X_END_OFF |
                this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
                KNIGHTPROMO_CAP );
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos + 1) << X_END_OFF |
                this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
                ROOKPROMO_CAP );
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos + 1) << X_END_OFF |
                this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
                BISHOPPROMO_CAP );
          }
        }
    }
    else
    {
      if(game_board->fields[this_piece->x_pos][this_piece->y_pos - 1] == nullptr) //don't need boundchecking here.
      {
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
              QUIET );
      }
      if(this_piece->x_pos != 0) //left capture
      {
        if(game_board->fields[this_piece->x_pos - 1][this_piece->y_pos - 1] != nullptr &&
         game_board->fields[this_piece->x_pos - 1][this_piece->y_pos - 1]-> colour == white)
        {
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos - 1) << X_END_OFF |
                this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
                CAPTURE );
        }
      }
      if(this_piece->x_pos != 7) //right capture
      {
        if(game_board->fields[this_piece->x_pos + 1][this_piece->y_pos - 1] != nullptr &&
         game_board->fields[this_piece->x_pos + 1][this_piece->y_pos - 1]-> colour == white)
        {
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos + 1) << X_END_OFF |
                this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 1) << Y_END_OFF |
                CAPTURE );
        }
      }
    }
  }
}

/* This function generates all moves for a knight
** without considering if the moves
** will make it's own king chess. */
static void find_pseudo_legal_knight_moves(piece *this_piece, board *game_board, std::vector<move> *moves)
{
  int x = this_piece->x_pos;
  int y = this_piece->y_pos;
  for(int8_t i = -2; i <= 2; i++)
  {
    if(i == 0) continue;
    for(int8_t j = -2; j <= 2; j++)
    {
      if(j==0 || abs(i) == abs(j)) continue;
      if(x + i >= 0 && x + i <= 7 && y + j >= 0 && y + j <= 7)
      {
        if(game_board->fields[x + i][y + j] == nullptr)
        {
          moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
            this_piece->y_pos << Y_START_OFF | uint8_t(y + j) << Y_END_OFF |
            QUIET );
        }
        else if (game_board->fields[x + i][y + j]->colour != this_piece->colour)
        {
          moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
            this_piece->y_pos << Y_START_OFF | uint8_t(y + j) << Y_END_OFF |
            CAPTURE );
        }
      }
    }
  }
}

/* This function generates all moves for a queen
** without considering if the moves
** will make it's own king chess. */
static void find_pseudo_legal_queen_moves(piece *this_piece, board *game_board, std::vector<move> *moves)
{
  find_pseudo_legal_rook_moves(this_piece, game_board, moves);
  find_pseudo_legal_bishop_moves(this_piece, game_board, moves);
}

/* This function generates all moves for a rook
** without considering if the moves
** will make it's own king chess.*/
static void find_pseudo_legal_rook_moves(piece *this_piece, board *game_board, std::vector<move> *moves)
{
  int x = this_piece->x_pos;
  int y = this_piece->y_pos;
  //left
  for(int8_t i = 1; i + x <= 7; i++)
  {
    if(game_board->fields[i + x][y] == nullptr)
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | this_piece->y_pos << Y_END_OFF |
        QUIET );
    }
    else if(game_board->fields[i + x][y]->colour == this_piece->colour)
      break;
    else
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | this_piece->y_pos << Y_END_OFF |
        CAPTURE );
      break;
    }
  }
  //right
  for(int8_t i = -1; i + x >= 0; i--)
  {
    if(game_board->fields[i + x][y] == nullptr)
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | this_piece->y_pos << Y_END_OFF |
        QUIET );
    }
    else if(game_board->fields[i + x][y]->colour == this_piece->colour)
      break;
    else
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | this_piece->y_pos << Y_END_OFF |
        CAPTURE );
      break;
    }
  }
  //up
  for(int8_t i = 1; i + y <= 7; i++)
  {
    if(game_board->fields[x][y + i] == nullptr)
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | uint8_t(y + i) << Y_END_OFF |
        QUIET );
    }
    else if(game_board->fields[x][y + i]->colour == this_piece->colour)
      break;
    else
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | uint8_t(y + i) << Y_END_OFF |
        CAPTURE );
      break;
    }
  }
  //down
  for(int8_t i = -1; i + y >= 0; i--)
  {
    if(game_board->fields[x][y + i] == nullptr)
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | uint8_t(y + i) << Y_END_OFF |
        QUIET );
    }
    else if(game_board->fields[x][y + i]->colour == this_piece->colour)
      break;
    else
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | uint8_t(y + i) << Y_END_OFF |
        CAPTURE );
      break;
    }
  }
}

static void find_pseudo_legal_bishop_moves(piece *this_piece, board *game_board, std::vector<move> *moves)
{
  int x = this_piece->x_pos;
  int y = this_piece->y_pos;
  //left up
  for(int8_t i = 1; i + x <= 7 && i + y <= 7; i++)
  {
    if(game_board->fields[i + x][i + y] == nullptr)
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | uint8_t(y + i) << Y_END_OFF |
        QUIET );
    }
    else if(game_board->fields[i + x][i + y]->colour == this_piece->colour)
      break;
    else
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | uint8_t(y + i) << Y_END_OFF |
        CAPTURE );
      break;
    }
  }
  //right down
  for(int8_t i = -1; i + x >= 0 && i + y >0; i--)
  {
    if(game_board->fields[i + x][i + y] == nullptr)
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | uint8_t(y + i) << Y_END_OFF |
        QUIET );
    }
    else if(game_board->fields[i + x][i + y]->colour == this_piece->colour)
      break;
    else
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | uint8_t(y + i) << Y_END_OFF |
        CAPTURE );
      break;
    }
  }
  //right up
  for(int8_t i = 1; i + x <= 7 && y - i >= 0 ; i++)
  {
    if(game_board->fields[x + i][y - i] == nullptr)
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | uint8_t(y - i) << Y_END_OFF |
        QUIET );
    }
    else if(game_board->fields[x + i][y - i]->colour == this_piece->colour)
      break;
    else
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | uint8_t(y - i) << Y_END_OFF |
        CAPTURE );
      break;
    }
  }
  //down left
  for(int8_t i = -1; i + x >= 0 && y - i <= 7 ; i--)
  {
    if(game_board->fields[x + i][y - i] == nullptr)
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | uint8_t(y - i) << Y_END_OFF |
        QUIET );
    }
    else if(game_board->fields[x + i][y - i]->colour == this_piece->colour)
      break;
    else
    {
      moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
        this_piece->y_pos << Y_START_OFF | uint8_t(y - i) << Y_END_OFF |
        CAPTURE );
      break;
    }
  }
}


/* This function generates all moves for a king
** without considering if the moves
** will make it chess.
static void find_pseudo_legal_king_moves(piece *this_piece, board *game_board, std::vector<move> *moves)
{

} */

void make_move(piece *moving_piece, board *game_board, move the_move)
{ //assume only valide moves are given. Just obey it mindlessly
  uint8_t move_end_x = (the_move & X_END_MASK) >> X_END_OFF;
  uint8_t move_end_y  = (the_move & Y_END_MASK) >> Y_END_OFF;
  uint8_t move_start_x = (the_move & X_START_MASK) >> X_START_OFF;
  uint8_t move_start_y  = (the_move & Y_START_MASK) >> Y_START_OFF;

  uint8_t move_type = the_move & MOVE_TYPE_MASK;
  switch(move_type)
  {
    case QUIET: case DOUBLEPAWN:
      //Just move the piece and update game_board
      moving_piece->move_to(move_end_x, move_end_y);
      game_board->fields[move_start_x][move_start_y] = nullptr;
      game_board->fields[move_end_x][move_end_y] = moving_piece;
      break;
    case CAPTURE: //set captured piece to dead, and move det capturing piece to the new field
      moving_piece->move_to(move_end_x, move_end_y);
      game_board->fields[move_start_x][move_start_y] = nullptr;
      game_board->fields[move_end_x][move_end_y]->alive = false;
      game_board->fields[move_end_x][move_end_y] = moving_piece;
      break;
    case QUEENPROMO_CAP:  //same as capture, but with promo
      moving_piece->move_to(move_end_x, move_end_y);
      game_board->fields[move_start_x][move_start_y] = nullptr;
      game_board->fields[move_end_x][move_end_y]->alive = false;
      game_board->fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(queen);
      break;
    case KNIGHTPROMO_CAP: //same as capture, but with promo
      moving_piece->move_to(move_end_x, move_end_y);
      game_board->fields[move_start_x][move_start_y] = nullptr;
      game_board->fields[move_end_x][move_end_y]->alive = false;
      game_board->fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(knight);
      break;
    case ROOKPROMO_CAP:   //same as capture, but with promo
      moving_piece->move_to(move_end_x, move_end_y);
      game_board->fields[move_start_x][move_start_y] = nullptr;
      game_board->fields[move_end_x][move_end_y]->alive = false;
      game_board->fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(rook);
      break;
    case BISHOPPROMO_CAP: //same as capture, but with promo
      moving_piece->move_to(move_end_x, move_end_y);
      game_board->fields[move_start_x][move_start_y] = nullptr;
      game_board->fields[move_end_x][move_end_y]->alive = false;
      game_board->fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(bishop);
      break;
    case QUEENPROMO:      //same as quiet, but with promo
      moving_piece->move_to(move_end_x, move_end_y);
      game_board->fields[move_start_x][move_start_y] = nullptr;
      game_board->fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(queen);
      break;
    case KNIGHTPROMO:     //same as quiet, but with promo
      moving_piece->move_to(move_end_x, move_end_y);
      game_board->fields[move_start_x][move_start_y] = nullptr;
      game_board->fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(knight);
      break;
    case ROOKPROMO:       //same as quiet, but with promo
      moving_piece->move_to(move_end_x, move_end_y);
      game_board->fields[move_start_x][move_start_y] = nullptr;
      game_board->fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(rook);
      break;
    case BISHOPPROMO:     //same as quiet, but with promo
      moving_piece->move_to(move_end_x, move_end_y);
      game_board->fields[move_start_x][move_start_y] = nullptr;
      game_board->fields[move_end_x][move_end_y] = moving_piece;
      moving_piece->change_type(bishop);
      break;
    case RIGHTCASTLE:
      break;
    case LEFTCASTLE:
      break;
  }

}
