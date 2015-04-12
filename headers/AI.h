#include "Player.h"
#include "Board.h"
#include <thread>
#include "Move.h"
#include <mutex>
#pragma once

class AI
{
  private:
    player *this_player;
    player *opponent;
    board *game_board;
    int32_t negamax(player *player1, player *player2, board *the_board);
    move get_best_move();
    std::thread *AI_thread;
    std::mutex *draw_mtx;
    bool stop;

  public:
    static int32_t issolated_pawns(player *player1, board * the_board);
    static int32_t doubled_pawns(player *player1, board * the_board);
    static int32_t blocked_pawns(player *player1, board * the_board);
    static int32_t pawn_score(player *player1, board * the_board);
    static int32_t evaluate(player *player1, player *player2, board *the_board);


    AI(player *this_player_, player *opponent_, board *game_board_, std::mutex *draw_mtx);
    ~AI();
    void AI_loop();
};
