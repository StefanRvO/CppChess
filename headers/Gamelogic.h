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

void find_pseudo_legal_quiet_moves(piece *this_piece, board *game_board, std::vector<move> *moves);
void find_legal_quiet_moves(piece *this_piece, player *this_player, board *game_board, std::vector<move> *moves);
void find_pseudo_legal_capture_moves(piece *this_piece, board *game_board, std::vector<move> *moves);
void find_legal_capture_moves(piece *this_piece, player *this_player, board *game_board, std::vector<move> *moves);
void find_legal_moves(piece *this_piece, player *this_player, board *game_board, std::vector<move> *moves);
void find_pseudo_legal_moves(piece *this_piece, board *game_board, std::vector<move> *moves);

bool is_under_attack(uint8_t x, uint8_t y, board *game_board, player_colour colour);
void print_board(board *game_board);
std::string get_board_string(board *game_board);
bool is_board_consistent(board *game_board);
void print_move(move the_move);
