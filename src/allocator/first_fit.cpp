#include "allocator.h"
#include <iostream>
using namespace std;

void malloc_first_fit(size_t request_size)
{
    alloc_requests++;
    for(auto it =memory_blocks.begin(); it!=memory_blocks.end(); ++it)
    {
        if(it->free && it->size >= request_size)
        {
            Block allocated;
            allocated.start=it->start;
            allocated.size=request_size;
            allocated.requested=request_size;
            allocated.free=false;
            allocated.id=next_block_id++;
            alloc_success++;

            size_t remaining_size= it->size - request_size;
            it = memory_blocks.erase(it);
            memory_blocks.insert(it, allocated);
            if(remaining_size >0)
            {
                Block remaining;
                remaining.start= allocated.start + request_size;
                remaining.size= remaining_size;
                remaining.requested=0;
                remaining.free=true;
                remaining.id=-1;
                memory_blocks.insert(it, remaining);
            }
            cout<<"Allocated block id="<<allocated.id
                << " at address 0x"
                << hex <<allocated.start<<dec<<endl;
            return;
        }    
    }
    cout<<"Allocation failed: Not enough memory\n";
}