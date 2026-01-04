#include "allocator.h"
#include <iostream>

using namespace std;

/*
 * Allocates memory using the Worst Fit strategy.
 *
 * The allocator scans all free memory blocks and selects the
 * largest available block that can satisfy the request.
 * The intention is to leave large leftover blocks in memory,
 * potentially reducing the creation of many small unusable fragments.
 *
 * If the selected block is larger than the request, it is split
 * into an allocated block and a remaining free block.
 */
void malloc_worst_fit(size_t size)
{
    // Iterator pointing to the largest suitable free block
    auto worst = memory_blocks.end();

    // Track allocation attempt
    alloc_requests++;

    // Search for the largest free block that can satisfy the request
    for (auto it = memory_blocks.begin(); it != memory_blocks.end(); ++it)
    {
        if (it->free && it->size >= size)
        {
            if (worst == memory_blocks.end() || it->size > worst->size)
                worst = it;
        }
    }

    // No suitable free block found
    if (worst == memory_blocks.end())
    {
        cout << "Allocation failed\n";
        return;
    }

    // Create allocated block
    Block alloc;
    alloc.start = worst->start;
    alloc.size = size;
    alloc.requested = size;
    alloc.free = false;
    alloc.id = next_block_id++;

    // Update allocation success counter
    alloc_success++;

    // Calculate remaining free space after allocation
    size_t remaining = worst->size - size;

    // Replace selected free block with allocated block
    worst = memory_blocks.erase(worst);
    memory_blocks.insert(worst, alloc);

    // Insert remaining free block if space remains
    if (remaining > 0)
    {
        Block rem;
        rem.start = alloc.start + size;
        rem.size = remaining;
        rem.requested = 0;
        rem.free = true;
        rem.id = -1;

        memory_blocks.insert(worst, rem);
    }

    // Report allocation result
    cout << "Allocated block id=" << alloc.id
         << " at address=0x"
         << hex << alloc.start << dec << "\n";
}
