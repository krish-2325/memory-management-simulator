#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <list>
#include <cstddef>
#include "common.h"

enum AllocatorType { FIRST_FIT, BEST_FIT, WORST_FIT };

// memory state
extern std::list<Block> memory_blocks;
extern size_t TOTAL_MEMORY;
extern int alloc_requests;
extern int alloc_success;
extern int next_block_id;
extern int total_memory_accesses;

// allocator API
void init_memory(size_t size);
void malloc_first_fit(size_t size);
void malloc_best_fit(size_t size);
void malloc_worst_fit(size_t size);
void free_block(int id);
void dump_memory();
void stats();

#endif
