#include "../headers/AI.h"
#include <chrono>
#include <vector>
#include "../headers/Move.h"
#include "../headers/Gamelogic.h"
#include <iostream>
#include <algorithm>

#ifdef DEBUG
int32_t searches = 0;
std::mutex searcheslock;
std::mutex searchdepth_lock;
int32_t maxdepth = 99999;
#endif

static void AI_loop_wrapper(AI *thisAI);


AI::AI(player *this_player_, player *opponent_, board *game_board_, std::mutex *draw_mtx_)
{
  stop = false;
  this_player = this_player_;
  opponent = opponent_;
  game_board = game_board_;
  draw_mtx = draw_mtx_;

  //create new thread with AILoop and stuff.
  AI_thread = new std::thread(AI_loop_wrapper,this);
}

AI::~AI()
{
  //destroy thread and stuff
  stop = true;
  AI_thread->join();
  delete AI_thread;
}



int32_t AI::alpha_beta(int32_t alpha, int32_t beta, player *white_player, player *black_player, board *the_board, int depth)
{
  int32_t best_score = -99999999;
  move best_move = 0xFFFF;
  if(depth == 0)
  {
    best_score = quiescence(alpha, beta, white_player, black_player, the_board, QUIESCDEPTH);
    return best_score;
  }
  std::vector<move> possible_moves;
  if(best_move != 0xFFFF) possible_moves.push_back(best_move);
  if(the_board->who2move == white)
  {
    for(uint8_t i = 0; i < 16; i++)
    {
      if(white_player->pieces[i].alive)
        find_legal_moves(&(white_player->pieces[i]), white_player, the_board, &possible_moves);
    }
  }
  else
  {
    for(uint8_t i = 0; i < 16; i++)
    {
      if(black_player->pieces[i].alive)
        find_legal_moves(&(black_player->pieces[i]), black_player, the_board, &possible_moves);
    }
  }
  for(auto the_move : possible_moves)
  {
    //perform the move and save some info about it so we can unmake it.
    uint8_t move_start_x = (the_move & X_START_MASK) >> X_START_OFF;
    uint8_t move_start_y  = (the_move & Y_START_MASK) >> Y_START_OFF;
    piece *moving_piece = the_board->fields[move_start_x][move_start_y];
    piece *targetpiece = the_board->make_move(moving_piece, the_move);

    auto score = - alpha_beta(-beta, -alpha, white_player, black_player, the_board, depth - 1);

    the_board->unmake_move(moving_piece, the_move, targetpiece);

    if(score >= beta)
    {
      return score;
    }
    if(score > best_score)
    {
      best_score = score;
      best_move = the_move;
      if(score > alpha)
      {
        alpha = score;
      }
    }
  }
  return alpha;
}



int32_t AI::quiescence(int32_t alpha, int32_t beta, player *white_player, player *black_player, board *the_board, int depth)
{ //perform a quiescence search
  #ifdef DEBUG
  searchdepth_lock.lock();
  if(maxdepth > depth) maxdepth = depth;
  searchdepth_lock.unlock();
  #endif
  int32_t stand_pat = evaluate(white_player, black_player, the_board);
  if(stand_pat >= beta) return beta;
  if(alpha < stand_pat) alpha = stand_pat;
  if(depth == 0) return stand_pat;

  //get capture moves
  std::vector<move> possible_moves;
  if(the_board->who2move == white)
  {
    for(uint8_t i = 0; i < 16; i++)
    {
      if(white_player->pieces[i].alive)
        find_legal_capture_moves(&(white_player->pieces[i]), white_player, the_board, &possible_moves);
    }
  }
  else
  {
    for(uint8_t i = 0; i < 16; i++)
    {
      if(black_player->pieces[i].alive)
        find_legal_capture_moves(&(black_player->pieces[i]), black_player, the_board, &possible_moves);
    }
  }
  for(auto the_move : possible_moves)
  {
    uint8_t move_start_x = (the_move & X_START_MASK) >> X_START_OFF;
    uint8_t move_start_y  = (the_move & Y_START_MASK) >> Y_START_OFF;
    piece *moving_piece = the_board->fields[move_start_x][move_start_y];
    piece *targetpiece = the_board->make_move(moving_piece, the_move);
    auto score = - quiescence(-beta, -alpha, white_player, black_player, the_board, depth - 1);
    the_board->unmake_move(moving_piece, the_move, targetpiece);
    if(score >= beta) return score;
    if (score > stand_pat)
    {
      stand_pat = score;
      if(score > alpha) alpha = score;
    }
  }
  return alpha;
}

move AI::get_best_move()
{
  move best_move = 0xFFFF;
  auto this_player_cpy = *this_player;
  auto opponent_cpy = *opponent;
  board game_board_copy(this_player_cpy, opponent_cpy, *game_board);
  player *white_player;
  player *black_player;
  if(this_player_cpy.colour == black) {black_player = &this_player_cpy; white_player = &opponent_cpy;}
  else                            {white_player = &this_player_cpy; black_player = &opponent_cpy;}

  std::vector<move> possible_moves;
  for(uint8_t i = 0; i < 16; i++)
  {
    if(this_player->pieces[i].alive)
    {
      find_legal_moves(&(this_player_cpy.pieces[i]), &this_player_cpy, &game_board_copy, &possible_moves);
    }
  }

  std::vector<move_score> possible_move_score_pairs;
  for(auto the_move : possible_moves)
  {
    move_score the_mv_sc;
    the_mv_sc.score = -9999999;
    the_mv_sc.the_move = the_move;
    possible_move_score_pairs.push_back(the_mv_sc);

  }
  for(int i = 0; i <= SEARCHDEPTH; i++)
  {
    int32_t beta = 99999999;
    int32_t alpha = -beta;
    int32_t best_score = -99999999;
    std::sort(possible_move_score_pairs.begin(), possible_move_score_pairs.end(), move_score_comp);
    for(auto &the_move_score : possible_move_score_pairs)
    {
      //perform the move and save some info about it so we can unmake it.
      uint8_t move_start_x = (the_move_score.the_move & X_START_MASK) >> X_START_OFF;
      uint8_t move_start_y  = (the_move_score.the_move & Y_START_MASK) >> Y_START_OFF;
      piece *moving_piece = game_board_copy.fields[move_start_x][move_start_y];
      piece *targetpiece = game_board_copy.make_move(moving_piece, the_move_score.the_move);

      auto score = - alpha_beta(-beta, -alpha, white_player, black_player, &game_board_copy, i);
      the_move_score.score = score;
      game_board_copy.unmake_move(moving_piece, the_move_score.the_move, targetpiece);

      if(score > best_score)
      {
        best_score = score;
        best_move = the_move_score.the_move;
        if(score > alpha)
        {
          alpha = score;
        }
      }
    }
  }
  return best_move;
}



int32_t AI::issolated_pawns(player *player1, board * the_board)
{
  int32_t total_issolated = 0;
  for(uint8_t i = 8; i < 16; i++)
  {
    bool issolated = true;
    if(player1->pieces[i].alive && player1->pieces[i].type == pawn)
    {
      if(player1->pieces[i].x_pos != 7)
      {
        for(uint8_t j = 0; j < 8; j++)
        {
          if(the_board->fields[player1->pieces[i].x_pos + 1][j] != nullptr &&
            the_board->fields[player1->pieces[i].x_pos + 1][j]->type == pawn &&
            the_board->fields[player1->pieces[i].x_pos + 1][j]->colour == player1->colour)
          {
            issolated = false;
            break;
          }
        }
      }
      if(issolated == true && player1->pieces[i].x_pos != 0)
      {
        for(uint8_t j = 0; j < 8; j++)
        {
          if(the_board->fields[player1->pieces[i].x_pos - 1][j] != nullptr &&
            the_board->fields[player1->pieces[i].x_pos - 1][j]->type == pawn &&
            the_board->fields[player1->pieces[i].x_pos - 1][j]->colour == player1->colour)
          {
            issolated = false;
            break;
          }
        }
      }
    }
    else issolated = false;
    if(issolated)
    {
      total_issolated += 1;
    }
  }
  return total_issolated;
}


int32_t AI::doubled_pawns(player *player1, board * the_board)
{
  int32_t total_doubled = 0;
  for(uint8_t i = 8; i < 16; i++)
  {
    bool doubled = false;
    if(player1->pieces[i].alive && player1->pieces[i].type == pawn)
    {
      //search y_pos for peasents
      for(uint8_t j = 0; j < 8; j++)
      {
        if(j == player1->pieces[i].y_pos) continue;
        if(the_board->fields[player1->pieces[i].x_pos][j] != nullptr &&
          the_board->fields[player1->pieces[i].x_pos][j]->type == pawn &&
          the_board->fields[player1->pieces[i].x_pos][j]->colour == player1->colour)
        {
          doubled = true;
          break;
        }
      }
      if(doubled)
      {
        total_doubled++;
      }
    }
  }
  return total_doubled;
}

int32_t AI::blocked_pawns(player *player1, board * the_board)
{
  int32_t total_blocked = 0;
  int8_t y_dir;
  if(player1->colour == white) y_dir = 1;
  else                         y_dir = -1;

  for(uint8_t i = 8; i < 16; i++)
  {
    if(player1->pieces[i].type != pawn || !player1->pieces[i].alive) continue;
    uint8_t x = player1->pieces[i].x_pos;
    uint8_t y = player1->pieces[i].y_pos;
    bool can_move_forward = false;
    bool can_attack_left  = false;
    bool can_attack_right = false;

    if(the_board->fields[x][y + y_dir] == nullptr)
    {
      can_move_forward = true;
    }
    else if(x != 0 && the_board->fields[x - 1][y + y_dir] != nullptr &&
      the_board->fields[x][y + y_dir]->colour != player1->colour)
    {
      can_attack_left = true;
    }
    else if(x != 7 && the_board->fields[x + 1][y + y_dir] != nullptr &&
      the_board->fields[x + 1][y + y_dir]->colour != player1->colour)
    {
      can_attack_right = true;
    }
    if(!(can_move_forward || can_attack_left || can_attack_right))
    {
      total_blocked += 1;
    }
  }
  return total_blocked;
}

int32_t AI::pawn_score(player *player1, board * the_board)
{
  int32_t score = 0;
  score += issolated_pawns(player1, the_board);
  score += blocked_pawns(player1, the_board);
  score += doubled_pawns(player1, the_board);
  return score;
}


int32_t AI::evaluate(player *white_player, player *black_player, board *the_board)
{
  int32_t score = 0;
  score = the_board->t_table->test_hash_eval( the_board->zob_hash);
  if(score != UNKNOWN_VAL)
  {
    return score;
  }
  else score = 0;
  #ifdef DEBUG
  searcheslock.lock();
  searches++;
  searcheslock.unlock();
  #endif

  //get piece score
  std::vector<move> possible_moves_black;
  std::vector<move> possible_moves_white;

  possible_moves_black.reserve(30);
  possible_moves_white.reserve(30);
  for(uint8_t i = 0; i <= 15; i++)
  {
    if(white_player->pieces[i].alive)
    {
      score += white_player->pieces[i].value;
      find_legal_moves(&(white_player->pieces[i]), white_player, the_board, &possible_moves_white);
    }
    if(black_player->pieces[i].alive)
    {
      score -= black_player->pieces[i].value;
      find_legal_moves(&(black_player->pieces[i]), black_player, the_board, &possible_moves_black);
    }
  }
  score += 1 * possible_moves_white.size();
  score -= 1 * possible_moves_black.size();

  score += -5 * pawn_score(white_player, the_board);
  score -= -5 * pawn_score(black_player, the_board);
  if(the_board->who2move == black) score = -score;
  the_board->t_table->save_hash_eval(score, the_board->zob_hash);


  //Need to do some stuff to special case chessmate and stalemate
  int chess_status = CheckChessMate(white_player, the_board);
  switch(chess_status)
  {
    case IS_CHESSMATE:
      score = -9999999;
      break;
    case IS_STALEMATE:
      score = 0;
      break;
  }
  chess_status = CheckChessMate(black_player, the_board);
  switch(chess_status)
  {
    case IS_CHESSMATE:
      score = 99999999;
      break;
    case IS_STALEMATE:
      score = 0;
      break;
  }
  return score;
}



void AI::AI_loop()
{
  while(!stop)
  {
    if(game_board->who2move == this_player->colour)
    {
      #ifdef DEBUG
      searches = 0;
      maxdepth = 99999;
      #endif
      move best_move = get_best_move();
      #ifdef DEBUG
      std::cout << "evaluations: " << searches << std::endl;
      std::cout << "maxdepth: " << SEARCHDEPTH + 1 + QUIESCDEPTH - maxdepth << std::endl;
      #endif
      if(best_move == 0xFFFF) return;
      auto start_x = (best_move & X_START_MASK) >> X_START_OFF;
      auto start_y = (best_move & Y_START_MASK) >> Y_START_OFF;
      piece *moving_piece = game_board->fields[start_x][start_y];
      draw_mtx->lock();
      game_board->make_final_move(moving_piece, best_move);
      draw_mtx->unlock();
      int chess_status = CheckChessMate(opponent, game_board);
      switch(chess_status)
      {
        case IS_CHESSMATE:
          if(this_player->colour == white)
            std::cout << "black is checkmate" << std::endl;
          else
            std::cout << "white is checkmate" << std::endl;
          break;
        case IS_STALEMATE:
          std::cout << "stalemate" << std::endl;
          break;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

static void AI_loop_wrapper(AI *thisAI)
{
  thisAI->AI_loop();
}
