#include "allocator.h"
#include <iostream>
using namespace std;

void malloc_worst_fit(size_t size) 
{
    auto worst = memory_blocks.end();
    alloc_requests++;

    for (auto it = memory_blocks.begin(); it != memory_blocks.end(); ++it) {
        if (it->free && it->size >= size) {
            if (worst == memory_blocks.end() || it->size > worst->size)
                worst = it;
        }
    }

    if (worst == memory_blocks.end()) {
        cout << "Allocation failed\n";
        return;
    }

    Block alloc;
    alloc.start = worst->start;
    alloc.size = size;
    alloc.requested = size;
    alloc.free = false;
    alloc.id = next_block_id++;
    alloc_success++;

    size_t remaining = worst->size - size;

    worst = memory_blocks.erase(worst);
    memory_blocks.insert(worst, alloc);

    if (remaining > 0) {
        Block rem;
        rem.start = alloc.start + size;
        rem.size = remaining;
        rem.requested = 0;
        rem.free = true;
        rem.id = -1;
        memory_blocks.insert(worst, rem);
    }

    cout << "Allocated block id=" << alloc.id << " at address=0x"
         << hex << alloc.start << dec << "\n";
}