#ifndef BUDDY_H
#define BUDDY_H

#include <map>
#include <vector>
#include <cstddef>

/*
 * BuddyAllocator
 *
 * Implements the Buddy Memory Allocation algorithm.
 * This allocator manages memory in blocks whose sizes are
 * powers of two, allowing efficient splitting and coalescing.
 *
 * Key properties:
 *  - Total memory size must be a power of two
 *  - Allocation sizes are rounded up to the nearest power of two
 *  - Each block has a unique "buddy" determined using XOR
 */
class BuddyAllocator {
public:
    /*
     * Configuration parameters
     */
    size_t total_size;   // Total managed memory size (power of two)
    size_t min_block;    // Smallest allocatable block size

    /*
     * Free lists indexed by block size.
     *
     * Each key represents a block size (2^k),
     * and the corresponding value is a list of starting addresses
     * of currently free blocks of that size.
     */
    std::map<size_t, std::vector<size_t>> free_lists;

    /*
     * Constructor
     *
     * Initializes the buddy allocator with:
     *  - A total memory size
     *  - A minimum block size
     *
     * Initially, the entire memory is one free block.
     */
    BuddyAllocator(size_t total, size_t min_block);

    /*
     * Allocate a memory block of at least `size` bytes.
     *
     * The requested size is rounded up to the nearest power of two.
     * If a larger block is used, it is recursively split until
     * the desired size is obtained.
     *
     * Returns:
     *  - Starting address of the allocated block
     *  - (size_t)-1 on allocation failure
     */
    size_t allocate(size_t size);

    /*
     * Free a previously allocated block.
     *
     * The allocator attempts to recursively merge the block
     * with its buddy (if free) to form larger blocks,
     * restoring larger free blocks where possible.
     */
    void free_block(size_t addr, size_t size);

    /*
     * Print the current state of all buddy free lists.
     *
     * Useful for debugging and demonstration purposes.
     */
    void dump();
};

#endif
