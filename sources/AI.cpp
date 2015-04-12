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

move AI::get_best_move()
{
  for(uint8_t i = 0; i <= 15; i++)
  {
    std::vector<move> possible_moves;
    find_legal_moves(&(this_player->pieces[i]), this_player, game_board, &possible_moves);
    if(possible_moves.size())
    {
      return possible_moves[0];
    }
  }
  return 0xFFFF;
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
            the_board->fields[player1->pieces[i].x_pos + 1][j]->alive == true &&
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
            the_board->fields[player1->pieces[i].x_pos - 1][j]->alive == true &&
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

  //get piece score
  std::vector<move> possible_moves_black;
  std::vector<move> possible_moves_white;

  possible_moves_black.reserve(100);
  possible_moves_white.reserve(100);
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
  return score;
}

void AI::AI_loop()
{
  while(!stop)
  {
    if(game_board->who2move == this_player->colour)
    {
      move best_move = get_best_move();
      auto start_x = (best_move & X_START_MASK) >> X_START_OFF;
      auto start_y = (best_move & Y_START_MASK) >> Y_START_OFF;
      piece *moving_piece = game_board->fields[start_x][start_y];
      draw_mtx->lock();
      make_move(moving_piece, game_board, best_move);
      draw_mtx->unlock();
      if(this_player->colour == black) game_board->who2move = white;
      else                             game_board->who2move = black;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

static void AI_loop_wrapper(AI *thisAI)
{
  thisAI->AI_loop();
}
