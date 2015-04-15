#include "Player.h"
#include "Board.h"
#include <thread>
#include "Move.h"
#include <mutex>
#define SEARCHDEPTH 3
#pragma once

class AI
{
  private:
    player *this_player;
    player *opponent;
    board *game_board;
    static int32_t negamax(player *player1, player *player2, board *the_board, int depth);
    move get_best_move();
    std::thread *AI_thread;
    std::mutex *draw_mtx;
    bool stop;
    static int32_t alpha_beta(int32_t alpha, int32_t beta, player *white_player, player *black_player, board *the_board, int depth);
    static int32_t quiescence(int32_t alpha, int32_t beta, player *white_player, player *black_player, board *the_board);
    static void get_best_move_piece_alpha_beta(uint8_t pieceid, player *player1, player *player2, board *the_board, move *best_move, int32_t *max, bool *success);
    static void get_best_move_piece(uint8_t pieceid, player *player1, player *player2, board *the_board, move *best_move, int32_t *max, bool *success);
  public:
    static void sort_moves_MVV_LVA(move *moves, int len, board *the_board);



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
