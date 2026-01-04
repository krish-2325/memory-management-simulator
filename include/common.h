#ifndef COMMON_H
#define COMMON_H

#include <cstddef>

/*
 * Block
 *
 * Represents a contiguous region of simulated physical memory.
 * This structure is shared by all allocation strategies
 * (First Fit, Best Fit, Worst Fit).
 */
struct Block {
    size_t start;        // Starting physical address of the block
    size_t size;         // Total size of the block in bytes
    size_t requested;    // Size originally requested by the user (for internal fragmentation)
    bool free;           // Indicates whether the block is free or allocated
    int id;              // Unique identifier for allocated blocks (-1 for free blocks)
};

#endif
