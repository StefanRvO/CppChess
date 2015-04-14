#pragma once
#include "Player.h"
#include "Piece.h"
#include "Move.h"
#include <string>
#include <vector>
#include "Trans_Table.h"

struct castling
{
  bool rights[2][2];
};

class board
{
  private:
  public:
    Trans_Table *t_table;
    piece *fields[8][8];
    player_colour who2move = white;
    board(player &white_p, player &black_p, Trans_Table *t_table_);
    board(player &white_p, player &black_p, board &old_board);
    ~board();
    bool is_board_consistent();
    void print_board();
    std::string get_board_string();
    piece *make_move(piece *moving_piece, move the_move);
    void make_final_move(piece *moving_piece, move the_move);
    void unmake_move(piece *moving_piece, move the_move, piece *second_piece);
    std::vector<move> moves;
    std::vector<castling> castling_rights_his;
    castling castling_rights;
    uint64_t zob_table_pieces[8][8][6][2]; //12*8*8 for pieces
    uint64_t zob_move;  //1 for who2move, 4 for castling.
    uint64_t zob_table_castling[2][2];
    uint64_t zob_hash;
    void init_zob_hash();
    void adjust_zob_hash_move(piece *moving_piece, piece *second_piece, move the_move);
    void adjust_zob_hash_unmove(piece *moving_piece, piece *second_piece, move the_move);


};
