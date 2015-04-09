#include "Player.h"
#include "Board.h"
#include <thread>
#pragma once

class AI
{
  private:
    player *this_player;
    player *opponent;
    board *game_board;
    uint32_t evaluate();
    uint32_t negamax();
    bool stop;

  public:
    AI(player *this_player_, player *opponent_, board *game_board_);
    ~AI();
    void AI_loop();
};
