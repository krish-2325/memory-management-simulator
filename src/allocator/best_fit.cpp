#include "allocator.h"
#include <iostream>
using namespace std;

void malloc_best_fit(size_t size) 
{
    auto best = memory_blocks.end();
    alloc_requests++;

    for (auto it = memory_blocks.begin(); it != memory_blocks.end(); ++it) {
        if (it->free && it->size >= size) {
            if (best == memory_blocks.end() || it->size < best->size)
                best = it;
        }
    }

    if (best == memory_blocks.end()) {
        cout << "Allocation failed\n";
        return;
    }

    Block alloc;
    alloc.start = best->start;
    alloc.size = size;
    alloc.requested = size;
    alloc.free = false;
    alloc.id = next_block_id++;
    alloc_success++;

    size_t original_size = best->size;
    best = memory_blocks.erase(best);
    memory_blocks.insert(best, alloc);
    size_t remaining = original_size - size;

    if (remaining > 0) {
        Block rem;
        rem.start = alloc.start + size;
        rem.size = remaining;
        rem.requested = 0;
        rem.free = true;
        rem.id = -1;
        memory_blocks.insert(best, rem);
    }

    cout << "Allocated block id=" << alloc.id << " at address=0x"
         << hex << alloc.start << dec << "\n";
}
