#pragma once
#include "Board.h"
#include "Player.h"
#include "Move.h"
#include "Timer.h"
#include "TextDrawer.h"
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#define WINDOWSIZE_X 600
#define WINDOWSIZE_Y 600
#define BLACK_FIELD_COLOUR 125,125,125,255
#define WHITE_FIELD_COLOUR 66 ,66 ,66 ,66
#define WHITE_PIECE 255,255,255,255
#define BLACK_PIECE 0  ,0  ,0  ,255
#define DRAWFPS 20
class drawer
{
  private:
    SDL_Window *window;
    SDL_Renderer* renderer;
    player *player1;
    player *player2;
    Timer timer;
    board  *game_board;
    bool stop = false;
    void draw_piece(piece *this_piece, TextDrawer *TDrawer, int w, int h);
  public:
    drawer(player *white_player, player *black_player, board *game_board_);
    ~drawer();
    void draw_possible_moves(std::vector<move> pos_moves);
    void draw_board();
    void loop();
};
