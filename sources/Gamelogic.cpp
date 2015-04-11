#include "../headers/Gamelogic.h"
#include <cstdlib>
#include <iostream>

static void find_pseudo_legal_pawn_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_knight_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_rook_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_bishop_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_queen_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_king_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

void find_legal_moves(piece *this_piece, player *this_player, board *game_board, std::vector<move> *moves)
{
  std::vector<move> pseudo_legal_moves;
  pseudo_legal_moves.reserve(20);
  find_pseudo_legal_moves(this_piece, game_board, &pseudo_legal_moves);
  std::cout << pseudo_legal_moves.size() << std::endl;
  for(auto this_move : pseudo_legal_moves)
  {
    //save info about move so it can be unmade.
    piece *targetpiece;
    uint8_t move_type = this_move & MOVE_TYPE_MASK;
    uint8_t move_end_x = (this_move & X_END_MASK) >> X_END_OFF;
    uint8_t move_end_y  = (this_move & Y_END_MASK) >> Y_END_OFF;
    switch(move_type)
    {
      case CAPTURE:
      case QUEENPROMO_CAP:
      case KNIGHTPROMO_CAP:
      case ROOKPROMO_CAP:
      case BISHOPPROMO_CAP:
        targetpiece = game_board->fields[move_end_x][move_end_y];
        break;
      case QUEEN_SIDE_CASTLE:
        targetpiece = game_board->fields[ROOK_0][move_end_y];
        break;
      case KING_SIDE_CASTLE:
        targetpiece = game_board->fields[ROOK_1][move_end_y];
        break;
    }
    //make the move
    make_move(this_piece, game_board, this_move);
    //check if king is in check
    if(!is_under_attack(this_player->pieces[KING].x_pos, this_player->pieces[KING].y_pos, game_board, this_player->colour))
    {
      moves->push_back(this_move);
      std::cout << "valid" << std::endl;
    }
    else
    {
      std::cout << "invalid" << std::endl;
    }

    //unmake move
    switch(move_type)
    {
      case QUIET:
      case DOUBLEPAWN:
      case QUEENPROMO:
      case KNIGHTPROMO:
      case ROOKPROMO:
      case BISHOPPROMO:
        unmake_move(this_piece, game_board, this_move);
        break;
      case CAPTURE:
      case QUEENPROMO_CAP:
      case KNIGHTPROMO_CAP:
      case ROOKPROMO_CAP:
      case BISHOPPROMO_CAP:
      case QUEEN_SIDE_CASTLE:
      case KING_SIDE_CASTLE:
        unmake_move(this_piece, game_board, this_move, targetpiece);
        break;
    }
  }
}

bool is_under_attack(uint8_t x, uint8_t y, board *game_board, player_colour colour)
{
  //search for bishop and queen which can attack this field.
  for(int8_t i = 1; i + x <= 7 && i + y <= 7; i++)
  {
    if(game_board->fields[i + x][i + y] == nullptr)
      continue;
    if(game_board->fields[i + x][i + y]->colour == colour || (game_board->fields[i + x][i + y]->type != bishop &&
        game_board->fields[i + x][i + y]->type != queen))
      break;
    else if(game_board->fields[i + x][i + y]->type == bishop || game_board->fields[i + x][i + y]->type == queen)
      return true;
  }
  //right down
  for(int8_t i = -1; i + x >= 0 && i + y >0; i--)
  {
    if(game_board->fields[i + x][i + y] == nullptr)
      continue;
    if(game_board->fields[i + x][i + y]->colour == colour || (game_board->fields[i + x][i + y]->type != bishop &&
        game_board->fields[i + x][i + y]->type != queen))
      break;
    else if(game_board->fields[i + x][i + y]->type == bishop || game_board->fields[i + x][i + y]->type == queen)
      return true;
  }
  //right up
  for(int8_t i = 1; i + x <= 7 && y - i >= 0 ; i++)
  {
    if(game_board->fields[x + i][y - i] == nullptr)
      continue;
    if(game_board->fields[i + x][y - i]->colour == colour || (game_board->fields[i + x][y - i]->type != bishop &&
        game_board->fields[i + x][y - i]->type != queen))
      break;
    else if(game_board->fields[i + x][y - i]->type == bishop || game_board->fields[i + x][y - i]->type == queen)
      return true;
  }
  //down left
  for(int8_t i = -1; i + x >= 0 && y - i <= 7 ; i--)
  {
    if(game_board->fields[x + i][y - i] == nullptr)
      continue;
    if(game_board->fields[i + x][y - i]->colour == colour || (game_board->fields[i + x][y - i]->type != bishop &&
        game_board->fields[i + x][y - i]->type != queen))
      break;
    else if(game_board->fields[i + x][y - i]->type == bishop || game_board->fields[i + x][y - i]->type == queen)
      return true;
  }



  /*****************************************************/
  //search for rook and queen

  //left
  for(int8_t i = 1; i + x <= 7; i++)
  {
    if(game_board->fields[i + x][y] == nullptr)
      continue;
    if(game_board->fields[i + x][y]->colour == colour || (game_board->fields[i + x][y]->type != rook &&
        game_board->fields[i + x][y]->type != queen))
      break;
    else if(game_board->fields[i + x][y]->type == rook || game_board->fields[i + x][y]->type == queen)
      return true;
  }
  //right
  for(int8_t i = -1; i + x >= 0; i--)
  {
    if(game_board->fields[i + x][y] == nullptr)
      continue;
    if(game_board->fields[i + x][y]->colour == colour || (game_board->fields[i + x][y]->type != rook &&
        game_board->fields[i + x][y]->type != queen))
      break;
    else if(game_board->fields[i + x][y]->type == rook || game_board->fields[i + x][y]->type == queen)
      return true;
  }
  //up
  for(int8_t i = 1; i + y <= 7; i++)
  {
    if(game_board->fields[x][y + i] == nullptr)
      continue;
    if(game_board->fields[x][y + i]->colour == colour || (game_board->fields[x][y + i]->type != rook &&
        game_board->fields[x][y + i]->type != queen))
      break;
    else if(game_board->fields[x][y + i]->type == rook || game_board->fields[x][y + i]->type == queen)
      return true;
  }
  //down
  for(int8_t i = -1; i + y >= 0; i--)
  {
    if(game_board->fields[x][y + i] == nullptr)
      continue;
    if(game_board->fields[x][y + i]->colour == colour || (game_board->fields[x][y + i]->type != rook &&
        game_board->fields[x][y + i]->type != queen))
      break;
    else if(game_board->fields[x][y + i]->type == rook || game_board->fields[x][y + i]->type == queen)
      return true;
  }

  /*****************************************************/


  //check for knights
  for(int8_t i = -2; i <= 2; i++)
  {
    if(i == 0) continue;
    for(int8_t j = -2; j <= 2; j++)
    {
      if(j==0 || abs(i) == abs(j)) continue;
      if(x + i >= 0 && x + i <= 7 && y + j >= 0 && y + j <= 7)
      {
        if(game_board->fields[x + i][y + j] == nullptr)
          continue;
        else if (game_board->fields[x + i][y + j]->type == knight && game_board->fields[x + i][y + j]-> colour != colour)
          return true;
      }
    }
  }

  /*****************************************************/


  //check for king
  for(int8_t i = -1; i <= 1; i++)
  {
    if(x + i < 0) continue;
    if(x + i > 7) break;
    for(int8_t j = -1; j <= 1; j++)
    {
      if(y + j < 0) continue;
      if(y + j > 7) break;
      if(j == 0 && i == 0) continue;
      if(game_board->fields[x + i][y + j] == nullptr)
        continue;
      else if(game_board->fields[x + i][y + j]->type == king && game_board->fields[x + i][y + j]->colour != colour)
        return true;
    }
  }

  /*****************************************************/

  //check for pawn
  if(colour == black)
  { //white pawns move towards 7
    if(int8_t(x) - 1 >= 0 && int8_t(y) - 1 >= 0 && game_board->fields[x - 1][y - 1] != nullptr &&
        game_board->fields[x - 1][y - 1]->type == pawn && game_board->fields[x - 1][y - 1]->colour == white)
      return true;
    if(x + 1 <= 7 && int8_t(y) - 1 >= 0 && game_board->fields[x + 1][y - 1] != nullptr &&
        game_board->fields[x + 1][y - 1]->type == pawn && game_board->fields[x + 1][y - 1]->colour == white)
      return true;
  }
  else
  { //black pawns moves towards 0
    if(int8_t(x) - 1 >= 0 && y + 1 <= 7 && game_board->fields[x - 1][y + 1] != nullptr &&
        game_board->fields[x - 1][y + 1]->type == pawn && game_board->fields[x - 1][y + 1]->colour == black)
      return true;
    if(x + 1 <= 7 && y + 1 <= 7 && game_board->fields[x + 1][y + 1] != nullptr &&
        game_board->fields[x + 1][y + 1]->type == pawn && game_board->fields[x + 1][y + 1]->colour == black)
      return true;
  }
  return false;
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
  	  break;
  	case king:
      find_pseudo_legal_king_moves(this_piece, game_board, moves);
      break;
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
** will make it chess. */
static void find_pseudo_legal_king_moves(piece *this_piece, board *game_board, std::vector<move> *moves)
{
  int x = this_piece->x_pos;
  int y = this_piece->y_pos;
  for(int8_t i = -1; i <= 1; i++)
  {
    if(x + i < 0) continue;
    if(x + i > 7) break;
    for(int8_t j = -1; j <= 1; j++)
    {
      if(y + j < 0) continue;
      if(y + j > 7) break;
      if(j == 0 && i == 0) continue;
    if(game_board->fields[x + i][y + j] == nullptr)
      {
        moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
          this_piece->y_pos << Y_START_OFF | uint8_t(y + j) << Y_END_OFF |
          QUIET );
      }
      else if(game_board->fields[x + i][y + j]->colour != this_piece->colour)
      {
        moves->push_back(this_piece->x_pos << X_START_OFF | uint8_t(x + i) << X_END_OFF |
          this_piece->y_pos << Y_START_OFF | uint8_t(y + j) << Y_END_OFF |
          CAPTURE );
      }
    }
  }

  //castling
  if(this_piece->moves == 0)
  {
    uint8_t y_pos;
    if(this_piece->colour == white) y_pos = 0;
    else                            y_pos = 7;
    //queenside
    if(game_board->fields[ROOK_0][y_pos] != nullptr && game_board->fields[ROOK_0][y_pos]->moves == 0)
    {
      //check if fields between king and rook is empty
      bool empty = true;
      for(uint8_t i = ROOK_0 + 1; i < KING; i++)
      {
        if(game_board->fields[i][y_pos] != nullptr )
        {
          empty = false;
          break;
        }
      }
      if(empty)
      {
        //check if king, or fields between king and rook is under attack
        bool attacked = false;
        for(uint8_t i = ROOK_0 + 1; i <= KING; i++)
        {
          if(is_under_attack(i, y_pos, game_board, this_piece->colour))
          {
            attacked = true;
            break;
          }
          if(!attacked)
          { //castling is allowed
            moves->push_back(this_piece->x_pos << X_START_OFF | (ROOK_0 + 2) << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | this_piece->y_pos << Y_END_OFF |
              QUEEN_SIDE_CASTLE );
          }
        }
      }
    }
    //kingside
    if(game_board->fields[ROOK_1][y_pos] != nullptr && game_board->fields[ROOK_1][y_pos]->moves == 0)
    {
      //check if fields between king and rook is empty
      bool empty = true;
      for(uint8_t i = ROOK_1 - 1; i > KING; i--)
      {
        if(game_board->fields[i][y_pos] != nullptr )
        {
          empty = false;
          break;
        }
      }
      if(empty)
      {
        //check if king, or fields between king and rook is under attack
        bool attacked = false;
        for(uint8_t i = ROOK_1 - 1; i >= KING; i--)
        {
          if(is_under_attack(i, y_pos, game_board, this_piece->colour))
          {
            attacked = true;
            break;
          }
          if(!attacked)
          { //castling is allowed
            moves->push_back(this_piece->x_pos << X_START_OFF | (ROOK_1 - 1) << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | this_piece->y_pos << Y_END_OFF |
              KING_SIDE_CASTLE );
          }
        }
      }
    }
  }
}

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
    case QUEEN_SIDE_CASTLE:
      moving_piece->move_to(move_end_x, move_end_y);
      game_board->fields[move_start_x][move_start_y] = nullptr;
      game_board->fields[move_end_x][move_end_y] = moving_piece;
      game_board->fields[ROOK_0 + 3][move_end_y] = game_board->fields[ROOK_0][move_end_y];
      game_board->fields[ROOK_0][move_end_y] = nullptr;
      game_board->fields[ROOK_0 + 3][move_end_y]->x_pos = ROOK_0 + 3;
      break;
    case KING_SIDE_CASTLE:
      moving_piece->move_to(move_end_x, move_end_y);
      game_board->fields[move_start_x][move_start_y] = nullptr;
      game_board->fields[move_end_x][move_end_y] = moving_piece;
      game_board->fields[ROOK_1 - 2][move_end_y] = game_board->fields[ROOK_1][move_end_y];
      game_board->fields[ROOK_1][move_end_y] = nullptr;
      game_board->fields[ROOK_1 - 2][move_end_y]->x_pos = ROOK_1 - 2;
      break;
  }
}

void unmake_move(piece *moving_piece, board *game_board, move the_move, piece *second_piece)
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
      game_board->fields[move_start_x][move_start_y] = moving_piece;
      game_board->fields[move_end_x][move_end_y] = second_piece;
      game_board->fields[move_end_x][move_end_y]->alive = false;
      break;
    case QUEENPROMO_CAP:
    case KNIGHTPROMO_CAP:
    case ROOKPROMO_CAP:
    case BISHOPPROMO_CAP:
      moving_piece->move_back_to(move_start_x, move_start_y);
      game_board->fields[move_start_x][move_start_y] = moving_piece;
      game_board->fields[move_end_x][move_end_y] = second_piece;
      game_board->fields[move_end_x][move_end_y]->alive = false;
      moving_piece->change_type(pawn);
      break;
    case QUEEN_SIDE_CASTLE:
      moving_piece->move_back_to(move_start_x, move_start_y);
      game_board->fields[move_start_x][move_start_y] = moving_piece;
      game_board->fields[move_end_x][move_end_y] = nullptr;
      game_board->fields[ROOK_0 - 2][move_end_y] = nullptr;
      game_board->fields[ROOK_0][move_end_y] = second_piece;
      second_piece->x_pos = ROOK_0;
      break;
    case KING_SIDE_CASTLE:
      moving_piece->move_back_to(move_start_x, move_start_y);
      game_board->fields[move_start_x][move_start_y] = moving_piece;
      game_board->fields[move_end_x][move_end_y] = nullptr;
      game_board->fields[ROOK_1 + 3][move_end_y] = nullptr;
      game_board->fields[ROOK_1][move_end_y] = second_piece;
      second_piece->x_pos = ROOK_1;
      break;
    default:
      std::cout << "Error in unmake move 1" << std::endl;
      break;
  }
}
void unmake_move(piece *moving_piece, board *game_board, move the_move)
{ //the moves which only affects one piece are quiet, doublepawn and promotions.

  uint8_t move_end_x = (the_move & X_END_MASK) >> X_END_OFF;
  uint8_t move_end_y  = (the_move & Y_END_MASK) >> Y_END_OFF;
  uint8_t move_start_x = (the_move & X_START_MASK) >> X_START_OFF;
  uint8_t move_start_y  = (the_move & Y_START_MASK) >> Y_START_OFF;

  uint8_t move_type = the_move & MOVE_TYPE_MASK;

  switch(move_type)
  {
    case QUIET:
    case DOUBLEPAWN:
      moving_piece->move_back_to(move_start_x, move_start_y);
      game_board->fields[move_start_x][move_start_y] = moving_piece;
      game_board->fields[move_end_x][move_end_y] = nullptr;
      break;
    case QUEENPROMO:
    case KNIGHTPROMO:
    case ROOKPROMO:
    case BISHOPPROMO:
      moving_piece->move_back_to(move_start_x, move_start_y);
      game_board->fields[move_start_x][move_start_y] = moving_piece;
      game_board->fields[move_end_x][move_end_y] = nullptr;
      moving_piece->change_type(pawn);
      break;
    default:
      std::cout << "Error in unmake move 2" << std::endl;
      break;

  }

}
