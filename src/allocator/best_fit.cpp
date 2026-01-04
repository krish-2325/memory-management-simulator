#include "allocator.h"
#include <iostream>

using namespace std;

/*
 * Allocates memory using the Best Fit strategy.
 *
 * The allocator scans all free memory blocks and selects the
 * smallest block that can satisfy the allocation request.
 * This minimizes internal fragmentation but requires a full
 * traversal of the memory block list.
 *
 * If the selected block is larger than the request, it is split
 * into an allocated block and a remaining free block.
 */
void malloc_best_fit(size_t size)
{
    // Iterator pointing to the best (smallest suitable) free block
    auto best = memory_blocks.end();

    // Track allocation attempt
    alloc_requests++;

    // Search for the smallest free block that fits the request
    for (auto it = memory_blocks.begin(); it != memory_blocks.end(); ++it)
    {
        if (it->free && it->size >= size)
        {
            if (best == memory_blocks.end() || it->size < best->size)
                best = it;
        }
    }

    // No suitable free block found
    if (best == memory_blocks.end())
    {
        cout << "Allocation failed\n";
        return;
    }

    // Create allocated block
    Block alloc;
    alloc.start = best->start;
    alloc.size = size;
    alloc.requested = size;
    alloc.free = false;
    alloc.id = next_block_id++;

    // Update allocation success counter
    alloc_success++;

    // Preserve original block size for splitting
    size_t original_size = best->size;

    // Replace the selected free block with the allocated block
    best = memory_blocks.erase(best);
    memory_blocks.insert(best, alloc);

    // Insert remaining free block if unused space remains
    size_t remaining = original_size - size;
    if (remaining > 0)
    {
        Block rem;
        rem.start = alloc.start + size;
        rem.size = remaining;
        rem.requested = 0;
        rem.free = true;
        rem.id = -1;

        memory_blocks.insert(best, rem);
    }

    // Report allocation result
    cout << "Allocated block id=" << alloc.id
         << " at address=0x"
         << hex << alloc.start << dec << "\n";
}
