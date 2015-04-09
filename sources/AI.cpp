#include "../headers/AI.h"
/*static void AI_loop_wrapper(AI *thisAI);*/


AI::AI(player *this_player_, player *opponent_, board *game_board_)
{
  stop = false;
  this_player = this_player_;
  opponent = opponent_;
  game_board = game_board_;

  //create new thread with AILoop and stuff.
}

AI::~AI()
{
  //destroy thread and stuff
  stop = true;
}

void AI::AI_loop()
{
  while(true); //do nothing atm.
}


/*
static void AI_loop_wrapper(AI *thisAI)
{
  thisAI->AI_loop();
}
*/
