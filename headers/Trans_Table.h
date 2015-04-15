#define TABLE_SIZE 100000 //Table size in entries
#define UNKNOWN_VAL -99999999
#include <mutex>
#include <cstdint>
#include "Move.h"
enum score_type
{
  INVALID = 0,
  EXACT = 1,
  ALPHA = 2,
  BETA = 3
};

#pragma once
struct table_entry
{
  uint64_t key;
  move best_move;
  int16_t depth;
  int32_t score;
  score_type type;
};

struct evaluate_entry
{
  uint64_t key[4];
  int32_t score[4];
  bool valid[4];
  uint8_t counter;
};

class Trans_Table
{
  public:
    table_entry *entries_depth;
    table_entry *entries_always_replace;
    evaluate_entry *evaluations;
    std::mutex *table_mtx;
    Trans_Table();
    ~Trans_Table();
    void clear();
    int32_t test_hash(int depth, int32_t alpha, int32_t beta, move *best_move, uint64_t zob_hash);
    void save_hash(int depth, int32_t score, score_type type, move best_move, uint64_t zob_hash );
    int32_t test_hash_eval(uint64_t zob_hash);
    void save_hash_eval(int32_t score, uint64_t zob_hash);

    int32_t used_entries();
};
