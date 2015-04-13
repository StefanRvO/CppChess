#pragma once
#include "Board.h"
#include "Player.h"
#include "Move.h"
#include "Timer.h"
#include "TextDrawer.h"
#include "Move.h"
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <mutex>
#define WINDOWSIZE_X 600
#define WINDOWSIZE_Y 600
#define BLACK_FIELD_COLOUR      125,125,125,255
#define WHITE_FIELD_COLOUR      66 ,66 ,66 ,255
#define QUIET_MOVE_COLOUR       20 ,125,20 ,255
#define CAPTURE_MOVE_COLOUR     125,20 ,20 ,255
#define PROMO_MOVE_COLOUR       20 ,20 ,125 ,255
#define SELECTED_PIECE_COLOUR   255 ,150 ,0 ,255
#define QUEEN_PROMO_SELECT      30  ,30  ,30,255
#define KNIGHT_PROMO_SELECT     60  ,60  ,60,255
#define ROOK_PROMO_SELECT       90  ,90  ,90,255
#define BISHOP_PROMO_SELECT     120 ,120 ,120,255

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
    void draw_piece(piece *this_piece, int w, int h);
    move select_move(player_colour player_to_select);
    void draw_pieces();
    void draw_possible_moves_board(std::vector<move> *possible_moves);
    move choose_promotion(move base_move);
    std::mutex *draw_mtx;
    SDL_Texture *piece_textures[2][6];
  public:
    drawer(player *white_player, player *black_player, board *game_board_, std::mutex *draw_mtx_);
    ~drawer();
    void draw_possible_moves(std::vector<move> pos_moves);
    void draw_board();
    void loop();
};
