#include "Player.h"
#include "Board.h"
#include <thread>
#include "Move.h"
#include <mutex>
#define SEARCHDEPTH         6
#define QUIESCDEPTH         20
#define WINDOW_SIZE         (PAWNVAL / 4)
#define TIMELIMIT 500000 //time to search for a move in µs
#pragma once

class AI
{
  private:
    player *this_player;
    player *opponent;
    board *game_board;
    move get_best_move();
    std::thread *AI_thread;
    std::mutex *draw_mtx;
    bool stop;
    static int32_t alpha_beta(int32_t alpha, int32_t beta, player *white_player, player *black_player, board *the_board, int depth);
    static int32_t quiescence(int32_t alpha, int32_t beta, player *white_player, player *black_player, board *the_board, int depth);

  public:
    static int32_t issolated_pawns(player *player1, board * the_board);
    static int32_t doubled_pawns(player *player1, board * the_board);
    static int32_t pawn_position_score(player *player1, board * the_board);
    static int32_t blocked_pawns(player *player1, board * the_board);
    static int32_t pawn_score(player *player1, board * the_board);
    static int32_t evaluate(player *player1, player *player2, board *the_board);
    

    AI(player *this_player_, player *opponent_, board *game_board_, std::mutex *draw_mtx);
    ~AI();
    void AI_loop();
};
