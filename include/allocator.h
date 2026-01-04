#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <list>
#include <cstddef>
#include "common.h"

/*
 * Supported dynamic memory allocation strategies.
 */
enum AllocatorType {
    FIRST_FIT,   // Allocate first sufficiently large free block
    BEST_FIT,    // Allocate smallest free block that fits
    WORST_FIT    // Allocate largest available free block
};

/*
 * Global allocator state
 *
 * These variables represent the simulated physical memory
 * and are shared across all allocation strategies.
 */

/*
 * Linked list representing the physical memory layout.
 * Each node corresponds to a contiguous memory block.
 */
extern std::list<Block> memory_blocks;

/*
 * Total size of simulated physical memory (bytes).
 */
extern size_t TOTAL_MEMORY;

/*
 * Allocation statistics.
 */
extern int alloc_requests;   // Total allocation requests issued
extern int alloc_success;    // Successful allocations
extern int next_block_id;    // Unique identifier for allocated blocks

/*
 * Total memory accesses (used for global summary reporting).
 */
extern int total_memory_accesses;

/*
 * Allocator API
 *
 * These functions provide a unified interface for memory
 * allocation, deallocation, visualization, and statistics.
 */

/*
 * Initialize the physical memory with a single free block.
 */
void init_memory(size_t size);

/*
 * Allocate memory using the First Fit strategy.
 */
void malloc_first_fit(size_t size);

/*
 * Allocate memory using the Best Fit strategy.
 */
void malloc_best_fit(size_t size);

/*
 * Allocate memory using the Worst Fit strategy.
 */
void malloc_worst_fit(size_t size);

/*
 * Free a previously allocated block using its block ID.
 * Automatically coalesces adjacent free blocks.
 */
void free_block(int id);

/*
 * Free a block by its starting physical address.
 */
void free_by_address(size_t address);

/*
 * Print a visualization of the current memory layout.
 */
void dump_memory();

/*
 * Print allocator statistics including:
 *  - Fragmentation metrics
 *  - Memory utilization
 *  - Allocation success rate
 */
void stats();

#endif
