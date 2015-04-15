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
  //Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if( !( IMG_Init( imgFlags ) & imgFlags ) )
  {
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "SDL error in PNG module!" << std::endl;
  }

  //load textures for the pieces
  	piece_textures[0][0] = IMG_LoadTexture(renderer, "images/king_black.png");
  	piece_textures[0][1] = IMG_LoadTexture(renderer, "images/knight_black.png");
  	piece_textures[0][2] = IMG_LoadTexture(renderer, "images/pawn_black.png");
  	piece_textures[0][3] = IMG_LoadTexture(renderer, "images/queen_black.png");
  	piece_textures[0][4] = IMG_LoadTexture(renderer, "images/rook_black.png");
    piece_textures[0][5] = IMG_LoadTexture(renderer, "images/bishop_black.png");
  	piece_textures[1][0] = IMG_LoadTexture(renderer, "images/king_white.png");
  	piece_textures[1][1] = IMG_LoadTexture(renderer, "images/knight_white.png");
  	piece_textures[1][2] = IMG_LoadTexture(renderer, "images/pawn_white.png");
  	piece_textures[1][3] = IMG_LoadTexture(renderer, "images/queen_white.png");
  	piece_textures[1][4] = IMG_LoadTexture(renderer, "images/rook_white.png");
    piece_textures[1][5] = IMG_LoadTexture(renderer, "images/bishop_white.png");

}

drawer::~drawer()
{
  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void drawer::draw_game_info()
{
  //draws info about the game in the right 20% of the screen.
  int w, h;
  SDL_GetWindowSize(window,&w,&h);
  int w_offset = w * 0.8;
  int fontsize = h;
  if( fontsize > w) fontsize = w;
  fontsize /= 14;

  //Draw whose turn it is.
  TextDrawer TDrawerTurn("FreeSans.ttf", fontsize);
  if(game_board->who2move == black)
    TDrawerTurn.DrawText(renderer, "BLACK", w_offset + w * 0.01, 0, 0, 255, 0, 255);
  else
    TDrawerTurn.DrawText(renderer, "WHITE", w_offset + w * 0.01, 0, 0, 255, 0, 255);

  //Draw if there is chessmate, chess or stalemate
  TextDrawer TDrawerChess("FreeSans.ttf", fontsize/2.5);
  int chess_status1 = CheckChessMate(player2, game_board);
  switch(chess_status1)
  {
    case IS_CHESSMATE:
      TDrawerChess.DrawText(renderer, "Black is checkmate!", w_offset + w * 0.01, h * 0.1, 255, 0, 0, 255);
      break;
    case IS_STALEMATE:
      TDrawerChess.DrawText(renderer, "Stalemate!",          w_offset + w * 0.01, h * 0.1, 255, 0, 0, 255);
      break;
  }
  int chess_status2 = CheckChessMate(player1, game_board);
  switch(chess_status2)
  {
    case IS_CHESSMATE:
        TDrawerChess.DrawText(renderer, "White is checkmate!", w_offset + w * 0.008, h * 0.1, 255, 0, 0, 255);
      break;
    case IS_STALEMATE:
      TDrawerChess.DrawText(renderer, "Stalemate!",          w_offset + w * 0.05, h * 0.1, 255, 0, 0, 255);
      break;
  }
  if(!chess_status1 && !chess_status2)
  {
    if(game_board->who2move == white)
    {
      if(isChess(player1, game_board))
      {
        TDrawerChess.DrawText(renderer, "White is check!", w_offset + w * 0.03, h * 0.1, 255, 0, 0, 255);
      }
    }
    else
    {
      if(isChess(player2, game_board))
      {
        TDrawerChess.DrawText(renderer, "Black is chess!", w_offset + w * 0.03, h * 0.1, 255, 0, 0, 255);
      }
    }

  }

  //Print the most resent moves
  TextDrawer TDrawerMoves("FreeSans.ttf", fontsize/2.5);

  //Lock mutex to prevent segfault
  draw_mtx->lock();
  TDrawerMoves.DrawText(renderer, "Recent Moves", w_offset + w * 0.03, h * 0.2, 0, 0, 255, 255);

  int i = 1;
  for(auto it = game_board->moves.rbegin(); it != game_board->moves.rend(); it++)
  {
    auto the_move = *it;
    int move_end_x = (the_move & X_END_MASK) >> X_END_OFF;
    int move_end_y  = (the_move & Y_END_MASK) >> Y_END_OFF;
    int move_start_x = (the_move & X_START_MASK) >> X_START_OFF;
    int move_start_y  = (the_move & Y_START_MASK) >> Y_START_OFF;
    int move_type = the_move & MOVE_TYPE_MASK;
    std::string move_string = "(";
    move_string += std::to_string(move_start_x);
    move_string += ",";
    move_string += std::to_string(move_start_y);
    move_string += ")->(";
    move_string += std::to_string(move_end_x);
    move_string += ",";
    move_string += std::to_string(move_end_y);
    move_string += ")";
    if(i % 2)
      TDrawerMoves.DrawText(renderer, std::string(std::to_string(i) + ": ").c_str(), w_offset + w * 0.03, h * 0.2 + 0.05 * h * i, 70, 70, 125, 255);
    else
      TDrawerMoves.DrawText(renderer, std::string(std::to_string(i) + ": ").c_str(), w_offset + w * 0.03, h * 0.2 + 0.05 * h * i, 125, 70, 70, 255);

    switch(move_type)
    {
    case QUIET: case DOUBLEPAWN:
      TDrawerMoves.DrawText(renderer, move_string.c_str(), w_offset + w * 0.055, h * 0.2 + 0.05 * h * i, 0, 255, 0, 255);
      break;
    case CAPTURE:
      TDrawerMoves.DrawText(renderer, move_string.c_str(), w_offset + w * 0.055, h * 0.2 + 0.05 * h * i, 255, 0, 0, 255);
      break;
    case QUEENPROMO_CAP:
    case KNIGHTPROMO_CAP:
    case ROOKPROMO_CAP:
    case BISHOPPROMO_CAP:
      TDrawerMoves.DrawText(renderer, move_string.c_str(), w_offset + w * 0.055, h * 0.2 + 0.05 * h * i, 255, 0, 255, 255);
      break;
    case QUEENPROMO:
    case KNIGHTPROMO:
    case ROOKPROMO:
    case BISHOPPROMO:
      TDrawerMoves.DrawText(renderer, move_string.c_str(), w_offset + w * 0.055, h * 0.2 + 0.05 * h * i, 0, 0, 255, 255);
      break;
    case QUEEN_SIDE_CASTLE:
    case KING_SIDE_CASTLE:
      TDrawerMoves.DrawText(renderer, move_string.c_str(), w_offset + w * 0.055, h * 0.2 + 0.05 * h * i, 125, 125, 125, 255);
      break;
    }
    i++;


  }

  draw_mtx->unlock();
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
    w = w * 0.8;
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
            int field_y = float(y) / h * 8;
            int field_x = float(x) / w * 8;
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
      SDL_SetRenderDrawColor(renderer, rand()%256, rand()%256, rand()%256, 255 );

      SDL_Rect r;
      r.x = w / 8 * i;
      r.y = h / 8 * 3;
      r.w = w / 8;
      r.h = h / 8;
      SDL_RenderFillRect( renderer, &r );

      SDL_Rect piece_rect;
      SDL_Texture *piece_texture_ptr = NULL;
      switch(i)
      {
        case 2:
          if(colour == black) piece_texture_ptr = piece_textures[0][3];
          else                            piece_texture_ptr = piece_textures[1][3];
          break;
        case 3:
          if(colour == black) piece_texture_ptr = piece_textures[0][1];
          else                            piece_texture_ptr = piece_textures[1][1];
          break;
        case 4:
          if(colour == black) piece_texture_ptr = piece_textures[0][4];
          else                            piece_texture_ptr = piece_textures[1][4];
          break;
        case 5:
          if(colour == black) piece_texture_ptr = piece_textures[0][5];
          else                            piece_texture_ptr = piece_textures[1][5];
          break;
      }
      piece_rect.w = w / 8 * 0.8;
      piece_rect.h = h / 8 * 0.8;
      piece_rect.x = w / 8 * i + w / 8 * 0.1;
      piece_rect.y = h / 8 * 3 + h / 8 * 0.1;

    	SDL_RenderCopy(renderer, piece_texture_ptr, NULL, &piece_rect);
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
    w = w * 0.8;
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
            int field_y = (7 - int(float(y) / h * 8));
            int field_x = float(x) / w * 8;

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
  w = w * 0.8;
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
    timer.tick();
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
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);
    draw_board();
    draw_pieces();
    draw_game_info();
    SDL_RenderPresent(renderer);
    if( (game_board-> who2move == white && player1->type == human) || (game_board-> who2move == black && player2->type == human))
    {
      SDL_SetRenderDrawColor(renderer,0,0,0,255);
      SDL_RenderClear(renderer);
      draw_board();
      draw_pieces();
      draw_game_info();
      SDL_RenderPresent(renderer);
      move selected_move = select_move(game_board-> who2move);
      if(selected_move == 0xFFFF) return;
      auto start_x = (selected_move & X_START_MASK) >> X_START_OFF;
      auto start_y = (selected_move & Y_START_MASK) >> Y_START_OFF;
      piece *moving_piece = game_board->fields[start_x][start_y];
      game_board->make_final_move(moving_piece, selected_move);
      if(game_board-> who2move == white)
      {
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
      else
      {
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
    }
  }
}

void drawer::draw_pieces()
{
  //Draw Pieces
  draw_mtx->lock();
  int w,h;
  SDL_GetWindowSize(window,&w,&h);
  w = w * 0.8;
  for(int i = 0; i < 8; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      if(game_board->fields[i][j] != nullptr)
      {
        draw_piece(game_board->fields[i][j], w, h);
      }
    }
  }
  draw_mtx->unlock();
}
void drawer::draw_board()
{
  int w,h;
  SDL_GetWindowSize(window,&w,&h);
  w = w * 0.8;
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

void drawer::draw_piece(piece *this_piece, int w, int h)
{ //This function uses the tenary operator. This operator may be unknown for some
  //The form is: (condition) ? (if_true) : (if_false)
  SDL_Rect piece_rect;
  SDL_Texture *piece_texture_ptr = NULL;
  switch(this_piece->type)
  {
    case pawn:
      if(this_piece->colour == black) piece_texture_ptr = piece_textures[0][2];
      else                            piece_texture_ptr = piece_textures[1][2];
      break;
    case knight:
      if(this_piece->colour == black) piece_texture_ptr = piece_textures[0][1];
      else                            piece_texture_ptr = piece_textures[1][1];
      break;
    case queen:
      if(this_piece->colour == black) piece_texture_ptr = piece_textures[0][3];
      else                            piece_texture_ptr = piece_textures[1][3];
      break;
    case bishop:
      if(this_piece->colour == black) piece_texture_ptr = piece_textures[0][5];
      else                            piece_texture_ptr = piece_textures[1][5];
      break;
    case rook:
      if(this_piece->colour == black) piece_texture_ptr = piece_textures[0][4];
      else                            piece_texture_ptr = piece_textures[1][4];
      break;
    case king:
      if(this_piece->colour == black) piece_texture_ptr = piece_textures[0][0];
      else                            piece_texture_ptr = piece_textures[1][0];
      break;
  }
  piece_rect.w = w / 8 * 0.8;
  piece_rect.h = h / 8 * 0.8;
  piece_rect.x = w / 8 * this_piece->x_pos + w / 8 * 0.1;
  piece_rect.y = h / 8 * (7- this_piece->y_pos) + h / 8 * 0.1;

	SDL_RenderCopy(renderer, piece_texture_ptr, NULL, &piece_rect);
}
