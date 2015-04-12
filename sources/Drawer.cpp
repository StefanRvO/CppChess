#include "../headers/Drawer.h"
#include <iostream>
#include <string>
#include "../headers/Gamelogic.h"
#include "../headers/Move.h"
#include <cstdlib>
#include "../headers/AI.h"
drawer::drawer(player *white_player, player *black_player, board *game_board_, std::mutex *draw_mtx_)
:timer(Timer(DRAWFPS))
{
  player1 = white_player;
  player2 = black_player;
  draw_mtx = draw_mtx_;
  game_board = game_board_;

  //Init SDL
  if(SDL_Init(SDL_INIT_VIDEO) != 0)
  {
      std::cout << "SDL_INIT Error: " << SDL_GetError() << std::endl;
      exit(0);
  }
  // Initialize SDL_ttf library
  if (TTF_Init() != 0)
  {
      std::cerr << "TTF_Init() Failed: " << TTF_GetError() << std::endl;
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
    std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    TTF_Quit();
    exit(0);
  }
  //Create Renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(renderer == nullptr)
  {
    SDL_DestroyWindow(window);
    std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
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
move drawer::choose_promotion(move base_move)
{
  int start_x = (base_move & X_START_MASK) >> X_START_OFF;
  int start_y = (base_move & Y_START_MASK) >> Y_START_OFF;
  player_colour colour = game_board->fields[start_x][start_y]->colour;

  while(true)
  {
    SDL_Event event; //grab events
    int y, x, w, h;
    SDL_GetWindowSize(window,&w,&h);

    while(SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
        {
          return 0xFFFF;
          break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
          if(event.button.button == SDL_BUTTON_LEFT)
          {
            y = event.button.y;
            x = event.button.x;

            //find the pressed field.
            int field_y = float(y) / w * 8;
            int field_x = float(x) / h * 8;
            auto move_type = base_move & MOVE_TYPE_MASK;
            if(move_type == QUEENPROMO || move_type == KNIGHTPROMO ||
                move_type == ROOKPROMO || move_type == BISHOPPROMO)
            {
              if(field_y == 3 && field_x == 2) return (base_move & ~MOVE_TYPE_MASK) | QUEENPROMO;
              if(field_y == 3 && field_x == 3) return (base_move & ~MOVE_TYPE_MASK) | KNIGHTPROMO;
              if(field_y == 3 && field_x == 4) return (base_move & ~MOVE_TYPE_MASK) | ROOKPROMO;
              if(field_y == 3 && field_x == 5) return (base_move & ~MOVE_TYPE_MASK) | BISHOPPROMO;
            }
            else
            {
              if(field_y == 3 && field_x == 2) return (base_move & ~MOVE_TYPE_MASK) | QUEENPROMO_CAP;
              if(field_y == 3 && field_x == 3) return (base_move & ~MOVE_TYPE_MASK) | KNIGHTPROMO_CAP;
              if(field_y == 3 && field_x == 4) return (base_move & ~MOVE_TYPE_MASK) | ROOKPROMO_CAP;
              if(field_y == 3 && field_x == 5) return (base_move & ~MOVE_TYPE_MASK) | BISHOPPROMO_CAP;
            }
          }
        }
      }
    }
    int fontsize = w;
    if (h > w) fontsize = h;
    fontsize /= 10;
    TextDrawer TDrawer("FreeSans.ttf", fontsize);
    std::string piece_string;
    for(int i = 2; i <= 5; i++)
    {
      if(i == 2)      SDL_SetRenderDrawColor(renderer, rand()%256, rand()%256, rand()%256, 255 );
      else if(i == 3) SDL_SetRenderDrawColor(renderer, rand()%256, rand()%256, rand()%256, 255);
      else if(i == 4) SDL_SetRenderDrawColor(renderer, rand()%256, rand()%256, rand()%256, 255);
      else if(i == 5) SDL_SetRenderDrawColor(renderer, rand()%256, rand()%256, rand()%256, 255);

      SDL_Rect r;
      r.x = w / 8 * i;
      r.y = h / 8 * 3;
      r.w = w / 8;
      r.h = h / 8;
      SDL_RenderFillRect( renderer, &r );

      switch(i)
      {
        case 2: piece_string = "Q"; break;
        case 3: piece_string = "H"; break;
        case 4: piece_string = "T"; break;
        case 5: piece_string = "R"; break;
      }
      if(colour == white)
        TDrawer.DrawText(renderer, piece_string.c_str(), w / 8 * i + w / 35, h / 8 * 3, WHITE_PIECE);
      else
        TDrawer.DrawText(renderer, piece_string.c_str(), w / 8 * i + w / 35, h / 8 * 3, BLACK_PIECE);
    }
    SDL_RenderPresent(renderer);
    timer.tick();
  }
}
move drawer::select_move(player_colour player_to_select)
{
  bool move_selected = false;
  move selected_move = 0xFFFF;
  std::vector<move> possible_moves;
  while(!move_selected)
  {
    SDL_Event event; //grab events
    int y, x, w, h;
    SDL_GetWindowSize(window,&w,&h);

    while(SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
        {
          return 0xFFFF;
          break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
          if(event.button.button == SDL_BUTTON_LEFT)
          {
            y = event.button.y;
            x = event.button.x;

            //find the pressed field.
            int field_y = (7 - int(float(y) / w * 8));
            int field_x = float(x) / h * 8;

            //check if pressed field is in possible moves.
            for(auto this_move : possible_moves)
            {
              int move_x = (this_move & X_END_MASK) >> X_END_OFF;
              int move_y = (this_move & Y_END_MASK) >> Y_END_OFF;
              if(move_x == field_x && move_y == field_y)
              {
                auto move_type = this_move & MOVE_TYPE_MASK;

                //Check if promotion
                if(move_type == QUEENPROMO || move_type == KNIGHTPROMO || move_type == ROOKPROMO ||
                    move_type == BISHOPPROMO || move_type == QUEENPROMO_CAP || move_type == KNIGHTPROMO_CAP
                    || move_type == ROOKPROMO_CAP || move_type == BISHOPPROMO_CAP)
                    {
                      //show promotion dialog
                      this_move = choose_promotion(this_move);
                    }
                  return this_move;
              }
            }
            if(!move_selected)
            { //Find moves for pressed field
              if(game_board->fields[field_x][field_y] != nullptr &&
                  game_board->fields[field_x][field_y]->colour == player_to_select)
              {
                possible_moves.clear();
                if(player_to_select == white)
                  find_legal_moves(game_board->fields[field_x][field_y], player1, game_board, &possible_moves);
                else
                  find_legal_moves(game_board->fields[field_x][field_y], player2, game_board, &possible_moves);
              }
            }
          }
          break;
        }
      }
    }
    draw_possible_moves_board( &possible_moves);
    draw_pieces();
    SDL_RenderPresent(renderer);
    timer.tick();

  }
  return selected_move;
}

void drawer::draw_possible_moves_board(std::vector<move> *possible_moves)
{
  draw_board();
  int w,h;
  SDL_GetWindowSize(window,&w,&h);
  //Draw the selected piece a colour
  if(possible_moves->size())
  {
    int x_pos = ((*possible_moves)[0] & X_START_MASK) >> X_START_OFF;
    int y_pos = ((*possible_moves)[0] & Y_START_MASK) >> Y_START_OFF;
    SDL_SetRenderDrawColor(renderer, SELECTED_PIECE_COLOUR);
    SDL_Rect r;
    r.x = w / 8 * x_pos;
    r.y = h / 8 * (7 - y_pos);
    r.w = w / 8;
    r.h = h / 8;
    SDL_RenderFillRect( renderer, &r );
  }
  for(auto this_move : *possible_moves)
  {
    int move_x = (this_move & X_END_MASK) >> X_END_OFF;
    int move_y = (this_move & Y_END_MASK) >> Y_END_OFF;
    auto move_type = this_move & MOVE_TYPE_MASK;
    //std::cout << move_x << "\t" << move_y << std::endl;

    if (move_type == QUIET || move_type == DOUBLEPAWN || move_type == QUEEN_SIDE_CASTLE ||
          move_type == KING_SIDE_CASTLE)
      SDL_SetRenderDrawColor(renderer, QUIET_MOVE_COLOUR);
    else if(move_type == CAPTURE || move_type == QUEENPROMO_CAP || move_type == KNIGHTPROMO_CAP ||
      move_type == ROOKPROMO_CAP || move_type == BISHOPPROMO_CAP)
      SDL_SetRenderDrawColor(renderer, CAPTURE_MOVE_COLOUR);
    else if(move_type == QUEENPROMO || move_type == KNIGHTPROMO || move_type == ROOKPROMO ||
      move_type == BISHOPPROMO )
      SDL_SetRenderDrawColor(renderer, PROMO_MOVE_COLOUR);
    SDL_Rect r;
    r.x = w / 8 * move_x;
    r.y = h / 8 * (7 - move_y);
    r.w = w / 8;
    r.h = h / 8;
    SDL_RenderFillRect( renderer, &r );
  }
}

void drawer::loop()
{
  while(!stop)
  {
    if(game_board-> who2move == white && player1->type == human)
    {
      move selected_move = select_move(white);
      if(selected_move == 0xFFFF) return;
      auto start_x = (selected_move & X_START_MASK) >> X_START_OFF;
      auto start_y = (selected_move & Y_START_MASK) >> Y_START_OFF;
      piece *moving_piece = game_board->fields[start_x][start_y];
      make_move(moving_piece, game_board, selected_move);
      game_board->who2move = black;
      int chess_status = CheckChessMate(player2, game_board);
      switch(chess_status)
      {
        case IS_CHESSMATE:
          std::cout << "black is checkmate" << std::endl;
          break;
        case IS_STALEMATE:
          std::cout << "stalemate" << std::endl;
          break;
      }
    }
    if(game_board-> who2move == black && player2->type == human)
    {
      move selected_move = select_move(black);
      if(selected_move == 0xFFFF) return;
      auto start_x = (selected_move & X_START_MASK) >> X_START_OFF;
      auto start_y = (selected_move & Y_START_MASK) >> Y_START_OFF;
      piece *moving_piece = game_board->fields[start_x][start_y];
      make_move(moving_piece, game_board, selected_move);
      game_board->who2move = white;
      int chess_status = CheckChessMate(player1, game_board);
      switch(chess_status)
      {
        case IS_CHESSMATE:
          std::cout << "white is checkmate" << std::endl;
          break;
        case IS_STALEMATE:
          std::cout << "stalemate" << std::endl;
          break;
      }
    }
    SDL_Event event; //grab events
    while(SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
          return;
          break;
      }
    }
    draw_board();
    draw_pieces();
    SDL_RenderPresent(renderer);
    timer.tick();
  }
}

void drawer::draw_pieces()
{
  //Draw Pieces
  draw_mtx->lock();
  int w,h;
  SDL_GetWindowSize(window,&w,&h);
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
        draw_piece(game_board->fields[i][j], &TDrawer, w, h);
      }
    }
  }
  draw_mtx->unlock();
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
      r.y = h / 8 * (7 - j);
      r.w = w / 8;
      r.h = h / 8;
      SDL_RenderFillRect( renderer, &r );
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
  //std::cout << (int)this_piece->x_pos << " " << (int)this_piece->y_pos << std::endl;
  if(this_piece->colour == white)
    TDrawer->DrawText(renderer, piece_string.c_str(), w / 8 * this_piece->x_pos + w / 35, h / 8 * (7 - this_piece->y_pos), WHITE_PIECE);
  else
    TDrawer->DrawText(renderer, piece_string.c_str(), w / 8 * this_piece->x_pos + w / 35, h / 8 * (7 - this_piece->y_pos), BLACK_PIECE);
}
