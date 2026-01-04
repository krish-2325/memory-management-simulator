#include "allocator.h"
#include <iostream>

using namespace std;

/*
 * Allocates memory using the First Fit strategy.
 *
 * The allocator scans the memory block list from the beginning
 * and selects the first free block that can satisfy the request.
 * If the selected block is larger than the request, it is split
 * into an allocated block and a remaining free block.
 */
void malloc_first_fit(size_t request_size)
{
    // Track allocation attempt
    alloc_requests++;

    // Traverse memory blocks in address order
    for (auto it = memory_blocks.begin(); it != memory_blocks.end(); ++it)
    {
        // Select the first free block large enough
        if (it->free && it->size >= request_size)
        {
            // Create allocated block
            Block allocated;
            allocated.start = it->start;
            allocated.size = request_size;
            allocated.requested = request_size;
            allocated.free = false;
            allocated.id = next_block_id++;

            // Update allocation success counter
            alloc_success++;

            // Calculate remaining space after allocation
            size_t remaining_size = it->size - request_size;

            // Replace original free block with allocated block
            it = memory_blocks.erase(it);
            memory_blocks.insert(it, allocated);

            // Insert remaining free block if space remains
            if (remaining_size > 0)
            {
                Block remaining;
                remaining.start = allocated.start + request_size;
                remaining.size = remaining_size;
                remaining.requested = 0;
                remaining.free = true;
                remaining.id = -1;

                memory_blocks.insert(it, remaining);
            }

            // Report allocation result
            cout << "Allocated block id=" << allocated.id
                 << " at address 0x"
                 << hex << allocated.start << dec << endl;

            return;
        }
    }

    // Allocation failure: no suitable free block found
    cout << "Allocation failed: Not enough memory\n";
}
