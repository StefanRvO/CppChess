#include "../headers/Board.h"
#include "../headers/Player.h"
#include "../headers/Drawer.h"
#include "../headers/AI.h"
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
using namespace std;
int main(int argv, char **args)
{
  player_type white_type = human;
  player_type black_type = human;
  for(int i = 1; i < argv; i++)
  {
    std::string arg_str = args[i];
    if      (arg_str == "-semiauto") {black_type = computer; white_type = human; break;}
    else if (arg_str == "-fullauto") {black_type = computer; white_type = computer; break;}
  }
  Trans_Table t_table;
  player white_player(white, white_type);
  player black_player(black, black_type);
  board game_board(white_player, black_player, &t_table);
  std::mutex draw_mtx;
  drawer ui_drawer(&white_player, &black_player, &game_board, &draw_mtx);
  std::vector<AI *> AIPlayers;
  if(white_player.type == computer)
  {
    AIPlayers.push_back(new AI(&white_player, &black_player, &game_board, &draw_mtx));
  }

  if(black_player.type == computer)
  {
    AIPlayers.push_back(new AI(&black_player, &white_player, &game_board, &draw_mtx));
  }
  ui_drawer.loop();
  return 0;
}
