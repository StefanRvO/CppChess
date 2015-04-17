#include "../headers/Trans_Table.h"
//inspired by code at http://web.archive.org/web/20070822204120/www.seanet.com/~brucemo/topics/hashing.htm
#include <iostream>
Trans_Table::Trans_Table()
{
  //Create the list
  #ifdef TRANS_TABLE
  entries_depth = new table_entry[TABLE_SIZE];
  entries_always_replace = new table_entry[TABLE_SIZE];
  #endif
  evaluations = new evaluate_entry[TABLE_SIZE];
  //create mutex
  table_mtx = new std::mutex;
  clear();
}

Trans_Table::~Trans_Table()
{
  #ifdef TRANS_TABLE
  delete[] entries_depth;
  delete[] entries_always_replace;
  #endif
  delete[] evaluations;
  delete table_mtx;
}

void Trans_Table::clear()
{ //set all types to invalid
  for(int i = 0; i < TABLE_SIZE; i++)
  {
    #ifdef TRANS_TABLE
    entries_depth[i].type = INVALID;
    entries_always_replace[i].type = INVALID;
    #endif
    evaluations[i].valid[0] = false;
    evaluations[i].valid[1] = false;
    evaluations[i].valid[2] = false;
    evaluations[i].valid[3] = false;
    evaluations[i].counter = 0;

  }
}


int32_t Trans_Table::test_hash_eval(uint64_t zob_hash)
{
  table_mtx->lock();
  auto this_entry = evaluations + (zob_hash % TABLE_SIZE);

  for(unsigned int i = 0; i < 4; i++)
  {
    if(this_entry->key[i] == zob_hash)
    {
      auto tmp = this_entry->score[i];
      table_mtx->unlock();
      return tmp;
    }
    else if(!this_entry->valid[i]) break;
  }
  table_mtx->unlock();
  return UNKNOWN_VAL;
}

void Trans_Table::save_hash_eval(int32_t score, uint64_t zob_hash)
{
  table_mtx->lock();
  auto *this_entry = evaluations + (zob_hash % TABLE_SIZE);
  this_entry->key[this_entry->counter] = zob_hash;
  this_entry->valid[this_entry->counter] = true;
  this_entry->score[this_entry->counter] = score;
  this_entry->counter++;
  if(this_entry->counter >= 4)
    this_entry->counter = 0;
  table_mtx->unlock();
}
#ifdef TRANS_TABLE
int32_t Trans_Table::test_hash(int depth, int32_t alpha, int32_t beta, move *best_move, uint64_t zob_hash)
{
  table_mtx->lock();
  table_entry *this_entry = entries_depth + (zob_hash % TABLE_SIZE);
  if(this_entry->key == zob_hash && this_entry->type != INVALID)
  {
    if(this_entry->depth >= depth)
    {
      if(this_entry->type == EXACT)
      {
        auto tmp = this_entry->score;
        table_mtx->unlock();
        return tmp;
      }
      if(this_entry->type == ALPHA && this_entry->score <= alpha)
      {
        table_mtx->unlock();
        return alpha;
      }
      if(this_entry->type == BETA && this_entry->score >= beta)
      {
        table_mtx->unlock();
        return beta;
      }
    }
    *best_move = this_entry->best_move;
  }
  else
  {
    this_entry = entries_always_replace + (zob_hash % TABLE_SIZE);
    if(this_entry->key == zob_hash && this_entry->type != INVALID)
    {
      if(this_entry->depth >= depth)
      {
        if(this_entry->type == EXACT)
        {
          auto tmp = this_entry->score;
          table_mtx->unlock();
          return tmp;
        }
        if(this_entry->type == ALPHA && this_entry->score <= alpha)
        {
          table_mtx->unlock();
          return alpha;
        }
        if(this_entry->type == BETA && this_entry->score >= beta)
        {
          table_mtx->unlock();
          return beta;
        }
      }
      *best_move = this_entry->best_move;
    }
  }
  table_mtx->unlock();
  return UNKNOWN_VAL;
}

void Trans_Table::save_hash(int depth, int32_t score, score_type type, move best_move, uint64_t zob_hash )
{
  table_mtx->lock();
  table_entry *this_entry = entries_depth + (zob_hash % TABLE_SIZE);
  if(this_entry->type == INVALID || this_entry->depth <= depth)
  {
    this_entry->key = zob_hash;
    this_entry->best_move = best_move;
    this_entry->score = score;
    this_entry->type = type;
    this_entry->depth = depth;
  }
  else
  {
    this_entry = entries_always_replace + (zob_hash % TABLE_SIZE);
    this_entry->key = zob_hash;
    this_entry->best_move = best_move;
    this_entry->score = score;
    this_entry->type = type;
    this_entry->depth = depth;
  }
  table_mtx->unlock();
}

int32_t Trans_Table::used_entries()
{
  int sum = 0;
  for(int i = 0; i < TABLE_SIZE; i++)
  {
    if(entries_depth[i].type != INVALID) sum++;
    if(entries_always_replace[i].type != INVALID) sum++;

  }
  return sum;
}
#endif
