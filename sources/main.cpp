#include "../headers/Board.h"
#include "../headers/Player.h"
#include "../headers/Drawer.h"
#include "../headers/AI.h"
#include <cstring>
#include <vector>
#include <iostream>
using namespace std;
int main(int argv, char **args)
{
  player_type white_type = human;
  player_type black_type = human;
  for(int i = 1; i < argv; i++)
  {
    if( strncmp(args[i], "-semiauto", 20)) {black_type = computer; white_type = human;}
    if( strncmp(args[i], "-fullauto", 20)) {black_type = computer; white_type = computer;}
  }
  player white_player(white, white_type);
  player black_player(black, black_type);
  board game_board(white_player, black_player);

  drawer ui_drawer(&white_player, &black_player, &game_board);
  std::vector<AI> AIPlayers;
  if(white_player.type == computer)
  {
    AIPlayers.push_back(AI(&white_player, &black_player, &game_board));
  }

  if(black_player.type == computer)
  {
    AIPlayers.push_back(AI(&black_player, &white_player, &game_board));
  }

  ui_drawer.loop();
}
