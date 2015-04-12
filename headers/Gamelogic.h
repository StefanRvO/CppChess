#pragma once

#define IS_CHESSMATE 1
#define IS_STALEMATE 2

#include "Player.h"
#include "Piece.h"
#include "Board.h"
#include "Move.h"
#include <vector>
#include <string>
bool isChess(player *this_player, board *game_board);
int  CheckChessMate(player *this_layer, board *game_board);

void find_pseudo_legal_moves(piece *this_piece, board *game_board, std::vector<move> *moves);
void find_legal_moves(piece *this_piece, player *this_player, board *game_board, std::vector<move> *moves);
bool is_under_attack(uint8_t x, uint8_t y, board *game_board, player_colour colour);
void make_move(piece *moving_piece, board *game_board, move the_move);
void unmake_move(piece *moving_piece, board *game_board, move the_move, piece *second_piece); //unmakes a move. a second piece can be giving if the move affected two pieces (captures, castling)
void unmake_move(piece *moving_piece, board *game_board, move the_move);
void print_board(board *game_board);
std::string get_board_string(board *game_board);
bool is_board_consistent(board *game_board);
