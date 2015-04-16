#include "../headers/AI.h"
#include <chrono>
#include <vector>
#include "../headers/Move.h"
#include "../headers/Gamelogic.h"
#include <iostream>

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


void AI::get_best_move_piece_alpha_beta(uint8_t pieceid, player *player1, player *player2, board *the_board, move *best_move, int32_t *max, bool *success)
{
  player player1_copy    = *player1;
  player player2_copy    = *player2;
  board game_board_copy(player1_copy, player2_copy, *the_board);
  game_board_copy.who2move = player1->colour;
  *max = -99999999;

  *best_move = 0xFFFF;
  player *white_player;
  player *black_player;
  int32_t alpha = -*max;
  //int32_t beta = *max;
  if(player1_copy.colour == white) {white_player = &player1_copy; black_player = &player2_copy;}
  else                              {black_player = &player1_copy; white_player = &player2_copy;}

  std::vector<move> possible_moves;
  possible_moves.reserve(100);
  if(game_board_copy.who2move == white)
  {
    find_legal_moves(&(white_player->pieces[pieceid]), white_player, &game_board_copy, &possible_moves);
  }
  else
  {
    find_legal_moves(&(black_player->pieces[pieceid]), black_player, &game_board_copy, &possible_moves);
  }

  for(auto the_move : possible_moves)
  {
    //perform the move and save some info about it so we can unmake it.
    uint8_t move_start_x = (the_move & X_START_MASK) >> X_START_OFF;
    uint8_t move_start_y  = (the_move & Y_START_MASK) >> Y_START_OFF;
    piece *moving_piece = game_board_copy.fields[move_start_x][move_start_y];
    piece *targetpiece = game_board_copy.make_move(moving_piece, the_move);


    auto score = - alpha_beta(-alpha, alpha, white_player, black_player, &game_board_copy, SEARCHDEPTH);

    game_board_copy.unmake_move(moving_piece, the_move, targetpiece);

    if(score > *max)
    {
      *max = score;
      *best_move = the_move;
      if(score > alpha)
      {
        alpha = score;
      }
    }
  }
  //hack to fix some weird bug
  if(possible_moves.size() == 1)
    *best_move = possible_moves[0];
  *success = possible_moves.size();
  the_board->t_table->save_hash(SEARCHDEPTH + 1, *max, EXACT, *best_move, game_board_copy.zob_hash);

}


move AI::get_best_move()
{
  move best_move = 0xFFFF;
  int32_t max = -99999999;
  move best_piece_moves[16];
  int32_t piece_maxs[16];
  bool success[16] = {false};
  std::vector<std::thread *> threads;

  for(uint8_t i = 0; i <= 15; i++)
  {
    if(this_player->pieces[i].alive)
    {
      auto number = threads.size();
      std::thread *piece_thread = new std::thread(AI::get_best_move_piece_alpha_beta, i, this_player, opponent, game_board, best_piece_moves + number, piece_maxs + number, success + number);
      threads.push_back(piece_thread);
    }
  }
  for(uint32_t i = 0; i < threads.size(); i++)
  {
    threads[i]->join();
    delete threads[i];
    if(success[i])
    {
      if(piece_maxs[i] > max || best_move == 0xFFFF)
      {
        max = piece_maxs[i];
        best_move = best_piece_moves[i];
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

int32_t AI::alpha_beta(int32_t alpha, int32_t beta, player *white_player, player *black_player, board *the_board, int depth)
{
  int32_t best_score = -99999999;
  //score_type type = ALPHA;
  move best_move = 0xFFFF;
  //if((best_score = the_board->t_table->test_hash(depth, alpha, beta, &best_move, the_board->zob_hash)) != UNKNOWN_VAL)
  //{
  //  return best_score;
  //}
  if(depth == 0)
  {
    best_score = quiescence(alpha, beta, white_player, black_player, the_board, QUIESCDEPTH);
    //the_board->t_table->save_hash(depth, best_score, EXACT, best_move, the_board->zob_hash);
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
  int i = 0;
  for(auto the_move : possible_moves)
  {
    if(i != 0 && the_move == possible_moves[0]) continue;
    i++;
    //perform the move and save some info about it so we can unmake it.
    uint8_t move_start_x = (the_move & X_START_MASK) >> X_START_OFF;
    uint8_t move_start_y  = (the_move & Y_START_MASK) >> Y_START_OFF;
    piece *moving_piece = the_board->fields[move_start_x][move_start_y];
    piece *targetpiece = the_board->make_move(moving_piece, the_move);

    auto score = - alpha_beta(-beta, -alpha, white_player, black_player, the_board, depth - 1);

    the_board->unmake_move(moving_piece, the_move, targetpiece);

    if(score >= beta)
    {
      //the_board->t_table->save_hash(depth, beta, BETA, best_move, the_board->zob_hash);
      return score;
    }
    if(score > best_score)
    {
      best_score = score;
      best_move = the_move;
      if(score > alpha)
      {
        //type = EXACT;
        alpha = score;
      }
    }
  }
  //the_board->t_table->save_hash(depth, alpha, type, best_move, the_board->zob_hash);
  return alpha;
}

int32_t AI::quiescence(int32_t alpha, int32_t beta, player *white_player, player *black_player, board *the_board, int depth)
{ //perform a quiescence search
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
    if(score >= beta) return beta;
    if (score > stand_pat)
    {
      stand_pat = score;
      if(score > alpha) alpha = score;
    }
  }
  return alpha;
}


void AI::AI_loop()
{
  while(!stop)
  {
    if(game_board->who2move == this_player->colour)
    {
      move best_move = get_best_move();
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
