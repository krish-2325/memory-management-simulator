#include "buddy.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

/*
 * Computes the smallest power of two greater than or equal to n.
 * Buddy allocation requires block sizes to be powers of two.
 */
static size_t next_power_of_two(size_t n)
{
    size_t p = 1;
    while (p < n)
        p <<= 1;
    return p;
}

/*
 * Constructs a Buddy Allocator.
 *
 * @param total  Total managed memory size (must be power of two)
 * @param minb   Minimum allocatable block size
 *
 * Initializes the allocator with a single free block covering
 * the entire memory region.
 */
BuddyAllocator::BuddyAllocator(size_t total, size_t minb)
    : total_size(total), min_block(minb)
{
    // Initialize free lists for each block size
    free_lists.clear();

    // Entire memory starts as one free block
    free_lists[total_size].push_back(0);
}

/*
 * Allocates memory using the Buddy allocation algorithm.
 *
 * The requested size is rounded up to the nearest power of two.
 * Blocks are split recursively until a block of the desired size
 * is obtained.
 *
 * @param size  Requested allocation size in bytes
 * @return Starting address of allocated block, or -1 on failure
 */
size_t BuddyAllocator::allocate(size_t size)
{
    // Round request to a power of two and enforce minimum block size
    size = next_power_of_two(size);
    if (size < min_block)
        size = min_block;

    size_t current = size;

    // Find the smallest available block size >= requested size
    while (free_lists[current].empty())
    {
        current <<= 1;
        if (current > total_size)
        {
            cout << "Buddy allocation failed\n";
            return (size_t)-1;
        }
    }

    // Remove a free block of the selected size
    size_t addr = free_lists[current].back();
    free_lists[current].pop_back();

    // Recursively split blocks until desired size is reached
    while (current > size)
    {
        current >>= 1;
        free_lists[current].push_back(addr + current);
    }

    cout << "Buddy allocated block at 0x"
         << hex << addr << dec
         << " size " << size << endl;

    return addr;
}

/*
 * Frees a previously allocated buddy block.
 *
 * Attempts to recursively coalesce the block with its buddy
 * using XOR-based address computation.
 *
 * @param addr  Starting address of the block to free
 * @param size  Size of the block being freed
 */
void BuddyAllocator::free_block(size_t addr, size_t size)
{
    // Normalize size to power of two
    size = next_power_of_two(size);
    size_t buddy;

    // Attempt buddy coalescing until maximum block size is reached
    while (size < total_size)
    {
        // Buddy address is computed using XOR
        buddy = addr ^ size;

        auto &list = free_lists[size];
        auto it = find(list.begin(), list.end(), buddy);

        // If buddy is not free, coalescing stops
        if (it == list.end())
            break;

        // Remove buddy from free list and merge blocks
        list.erase(it);
        addr = min(addr, buddy);
        size <<= 1;
    }

    // Insert the merged block back into the appropriate free list
    free_lists[size].push_back(addr);
}

/*
 * Dumps the current state of all buddy free lists.
 * This provides a visualization of memory fragmentation
 * and block availability.
 */
void BuddyAllocator::dump()
{
    cout << "\n--- Buddy Free Lists ---\n";
    for (auto &p : free_lists)
    {
        cout << "Size " << p.first << ": ";
        for (auto a : p.second)
            cout << "0x" << hex << a << " ";
        cout << dec << endl;
    }
}
