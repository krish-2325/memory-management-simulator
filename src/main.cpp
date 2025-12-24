#include <iostream>
#include <list>
#include <iomanip>
#include <string>
#include "common.h"

using namespace std;

//Global memory list
list<Block> memory_blocks;
size_t Totalmemory=0;
int next_block_id=1;
enum AllocatorType { FIRST_FIT, BEST_FIT, WORST_FIT };
AllocatorType current_allocator = FIRST_FIT;


//Initialize memory 
void innit_memeory (size_t size)
{
    memory_blocks.clear();
    Totalmemory=size;
    Block initial;
    initial.start=0;
    initial.size=size;  
    initial.free=true;
    initial.id=-1;
    memory_blocks.push_back(initial);
    cout<<"Memory initialized with size: " << size << " bytes" << endl;
}

void malloc_first_fit(size_t request_size)
{
    for(auto it =memory_blocks.begin(); it!=memory_blocks.end(); ++it)
    {
        if(it->free && it->size >= request_size)
        {
            Block allocated;
            allocated.start=it->start;
            allocated.size=request_size;
            allocated.free=false;
            allocated.id=next_block_id++;

            size_t remaining_size= it->size - request_size;
            it = memory_blocks.erase(it);
            memory_blocks.insert(it, allocated);
            if(remaining_size >0)
            {
                Block remaining;
                remaining.start= allocated.start + request_size;
                remaining.size= remaining_size;
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

void malloc_best_fit(size_t size) {
    auto best = memory_blocks.end();

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

    Block alloc = {best->start, size, false, next_block_id++};
    size_t remaining = best->size - size;

    best = memory_blocks.erase(best);
    memory_blocks.insert(best, alloc);

    if (remaining > 0) {
        Block rem = {alloc.start + size, remaining, true, -1};
        memory_blocks.insert(best, rem);
    }

    cout << "Allocated block id=" << alloc.id << " at address=0x"
         << hex << alloc.start << dec << "\n";
}

void malloc_worst_fit(size_t size) {
    auto worst = memory_blocks.end();

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

    Block alloc = {worst->start, size, false, next_block_id++};
    size_t remaining = worst->size - size;

    worst = memory_blocks.erase(worst);
    memory_blocks.insert(worst, alloc);

    if (remaining > 0) {
        Block rem = {alloc.start + size, remaining, true, -1};
        memory_blocks.insert(worst, rem);
    }

    cout << "Allocated block id=" << alloc.id << " at address=0x"
         << hex << alloc.start << dec << "\n";
}

//Free memory block
void free_block(int id)
{
    for(auto it= memory_blocks.begin(); it!= memory_blocks.end(); ++it)
    {
        if(!it->free && it->id == id)
        {
            it->free=true;
            it->id=-1;

            //Coalesce with previous
            if(it!=memory_blocks.begin())
            {
                auto prev = it;
                --prev;
                if(prev->free)
                {
                    prev->size += it->size;
                    it = memory_blocks.erase(it);
                    it = prev;
                }
            }

            //Coalesce with next
            auto next = it;
            ++next;
            if(next != memory_blocks.end() && next->free)
            {
                it->size += next->size;
                memory_blocks.erase(next);
            }
            cout<<"Block "<<id<<" freed merged\n";
            return;
        }
    }
    cout<<"No allocated block with id = "<<id<<endl;
}
//Dump memory layout 
void dump_memory()
{
   cout<<"\nMemory Layout:\n";
   for(auto &block : memory_blocks)
   {
       cout<<"[0x"
           << hex<< setw(4)<< setfill('0')<< block.start
           << " - 0x"
           <<setw(4) << (block.start + block.size - 1)
           <<"] ";
         if(block.free)
             cout<<"Free\n";
         else
             cout<<"USED (id=" << dec << block.id << ")\n";

        cout<<dec;     
   }
}

int main()
{
    string command;
    cout<<"Memory Simulator Started\n";
    while(true)
    {
        cout<<"> ";
        cin>>command;
        if(command=="init")
        {
            string temp;
            size_t size;
            cin>>temp>>size;
            innit_memeory(size);
        }
        else if(command=="dump")
        {
            dump_memory();
        }
        else if(command=="exit")
        {
            break;
        }
        else if (command == "malloc") 
        {
            size_t size;
            cin >> size;
            if (current_allocator == FIRST_FIT)
                malloc_first_fit(size);
            else if (current_allocator == BEST_FIT)
                malloc_best_fit(size);
            else if (current_allocator == WORST_FIT)
                malloc_worst_fit(size);
        }
        else if (command == "free") {
            int id;
            cin >> id;
            free_block(id);
        }
        else if (command == "set")
        {
            string temp, type;
            cin>>temp>>type;
            if(type=="first_fit")
                current_allocator=FIRST_FIT;
            else if(type=="best_fit")
                current_allocator=BEST_FIT;
            else if(type=="worst_fit")
                current_allocator=WORST_FIT;
            else
                cout<<"Unknown allocator type\n";
            cout<<"Allocator set to "<<type<<endl;
        }
        else
        {
            cout<<"Unknown command: "<<endl;
        }
    }
    return 0;
}