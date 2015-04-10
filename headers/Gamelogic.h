#pragma once

#include "Player.h"
#include "Piece.h"
#include "Board.h"
#include "Move.h"
#include <vector>
void isChess(player *this_player, player *opponent, board *game_board);
void isChessMate(player *this_layer, player *opponent, board *game_board);
void find_pseudo_legal_moves(piece *this_piece, player *this_player, player *opponent, board *game_board, std::vector<move> *moves);
void find_legal_moves(piece *this_piece, player *this_player, player *opponent, board *game_board, std::vector<move> moves);
