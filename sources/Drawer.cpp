#include "../headers/Drawer.h"
#include <iostream>
#include <string>
using namespace std;
drawer::drawer(player *white_player, player *black_player, board *game_board_)
:timer(Timer(DRAWFPS))
{
  player1 = white_player;
  player2 = black_player;

  game_board = game_board_;

  //Init SDL
  if(SDL_Init(SDL_INIT_VIDEO) != 0)
  {
      cout << "SDL_INIT Error: " << SDL_GetError() << endl;
      exit(0);
  }
  // Initialize SDL_ttf library
  if (TTF_Init() != 0)
  {
      cerr << "TTF_Init() Failed: " << TTF_GetError() << endl;
      SDL_Quit();
      TTF_Quit();
      exit(1);
  }

  //Create a window
  window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                      WINDOWSIZE_X, WINDOWSIZE_Y,
                      SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/);
  if(window == nullptr)
  {
    cout << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
    SDL_Quit();
    TTF_Quit();
    exit(0);
  }
  //Create Renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(renderer == nullptr)
  {
    SDL_DestroyWindow(window);
    cout << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
    SDL_Quit();
    TTF_Quit();
  }
}

drawer::~drawer()
{
  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void drawer::loop()
{
  while(!stop)
  {
    draw_board();
    SDL_RenderPresent(renderer);
    timer.tick();
  }
}

void drawer::draw_board()
{
  int w,h;
  SDL_GetWindowSize(window,&w,&h);

  //Draw Fields
  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      if((j % 2 == 0 && i % 2 == 0) || ( j % 2 == 1 && i % 2 == 1))
        SDL_SetRenderDrawColor(renderer, BLACK_FIELD_COLOUR);
      else
        SDL_SetRenderDrawColor(renderer, WHITE_FIELD_COLOUR);
      SDL_Rect r;
      r.x = w / 8 * i;
      r.y = h / 8 * j;
      r.w = w / 8;
      r.h = h / 8;
      SDL_RenderFillRect( renderer, &r );
    }
  }

  //Draw Pieces
  int fontsize = w;
  if (h > w) fontsize = h;
  fontsize /= 10;
  TextDrawer TDrawer("FreeSans.ttf", fontsize);
  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      if(game_board->fields[i][j] != nullptr)
      {
        //TDrawer.DrawText(renderer,"T", w / 8 * game_board->fields[i][j]->x_pos + w / 35, h / 8 * game_board->fields[i][j]->y_pos, WHITE_PIECE);
        draw_piece(game_board->fields[i][j], &TDrawer, w, h);
      }
    }
  }
}

void drawer::draw_piece(piece *this_piece, TextDrawer *TDrawer, int w, int h)
{ //This function uses the tenary operator. This operator may be unknown for some
  //The form is: (condition) ? (if_true) : (if_false)
  std::string piece_string = "";
  switch(this_piece->type)
  {
    case pawn:
      piece_string = "P";
      break;
    case knight:
      piece_string = "H";
      break;
    case queen:
      piece_string = "Q";
      break;
    case bishop:
      piece_string = "R";
      break;
    case rook:
      piece_string = "T";
      break;
    case king:
      piece_string = "K";
      break;
  }
  //cout << (int)this_piece->x_pos << " " << (int)this_piece->y_pos << endl;
  if(this_piece->colour == white)
    TDrawer->DrawText(renderer, piece_string.c_str(), w / 8 * this_piece->x_pos + w / 35, h / 8 * this_piece->y_pos, WHITE_PIECE);
  else
    TDrawer->DrawText(renderer, piece_string.c_str(), w / 8 * this_piece->x_pos + w / 35, h / 8 * this_piece->y_pos, BLACK_PIECE);
}
