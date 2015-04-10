#include "../headers/Gamelogic.h"
static void find_pseudo_legal_pawn_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_knight_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_queen_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_rook_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

static void find_pseudo_legal_king_moves(piece *this_piece, board *game_board, std::vector<move> *moves);


/* This function generates all moves
** without considering if the moves
** will make it's own king chess. */
void find_pseudo_legal_moves(piece *this_piece, player *this_player,
      player *opponent, board *game_board, std::vector<move> *moves)
{
  switch(this_piece->type)
  {
  	case pawn:
      find_pseudo_legal_pawn_moves(this_piece, game_board, moves);
      break;
  	case knight:
      find_pseudo_legal_knight_moves(this_piece, game_board, moves);
      break;
  	case queen:
      find_pseudo_legal_queen_moves(this_piece, game_board, moves);
  	  break;
    case bishop:
      find_pseudo_legal_bishop_moves(this_piece, game_board, moves);
      break;
  	case rook:
      find_pseudo_legal_rook_moves(this_piece, game_board, moves);
      break;
  	case king:
      find_pseudo_legal_king_moves(this_piece, game_board, moves);
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
      if(game_board->fields[this_piece->x_pos][this_piece->y_pos + 2] == nullptr) //don't need boundchecking here
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos + 2) << Y_END_OFF |
              DOUBLEPAWN );
    }
    else if(this_piece->y_pos == 6) // check for promotions
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
      if(game_board->fields[this_piece->x_pos][this_piece->y_pos - 2] == nullptr) //don't need boundchecking here
        moves->push_back(this_piece->x_pos << X_START_OFF | this_piece->x_pos << X_END_OFF |
              this_piece->y_pos << Y_START_OFF | (this_piece->y_pos - 2) << Y_END_OFF |
              DOUBLEPAWN );
    }
    else if(this_piece->y_pos == 1) // check for promotions
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
         game_board->fields[this_piece->x_pos + 1][this_piece->y_pos - 1]-> colour == black)
        {
          moves->push_back(this_piece->x_pos << X_START_OFF | (this_piece->x_pos - 1) << X_END_OFF |
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

}

/* This function generates all moves for a queen
** without considering if the moves
** will make it's own king chess. */
static void find_pseudo_legal_queen_moves(piece *this_piece, board *game_board, std::vector<move> *moves)
{

}

/* This function generates all moves for a rook
** without considering if the moves
** will make it's own king chess. */
static void find_pseudo_legal_rook_moves(piece *this_piece, board *game_board, std::vector<move> *moves)
{

}

/* This function generates all moves for a king
** without considering if the moves
** will make it chess. */
static void find_pseudo_legal_king_moves(piece *this_piece, board *game_board, std::vector<move> *moves)
{

}
