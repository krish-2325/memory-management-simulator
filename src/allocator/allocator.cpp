#include <iostream>
#include <iomanip>
#include <string>
#include "allocator.h"

using namespace std;

list<Block> memory_blocks;
size_t TOTAL_MEMORY=0;
int next_block_id=1;
int alloc_requests = 0;
int alloc_success = 0;

void init_memory (size_t size)
{
    memory_blocks.clear();
    TOTAL_MEMORY=size;
    Block initial;
    initial.start=0;
    initial.size=size;
    initial.requested=0;  
    initial.free=true;
    initial.id=-1;
    memory_blocks.push_back(initial);
    cout<<"Memory initialized with size: " << size << " bytes" << endl;
}

//Free memory block
void free_block(int id)
{
    for(auto it= memory_blocks.begin(); it!= memory_blocks.end(); ++it)
    {
        if(!it->free && it->id == id)
        {
            it->free=true;
            it->requested=0;
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
            cout<<"Block "<<id<<" freed and merged\n";
            return;
        }
    }
    cout<<"No allocated block with id = "<<id<<endl;
}

void free_by_address(size_t address)
{
    for (auto it = memory_blocks.begin(); it != memory_blocks.end(); ++it)
    {
        if (!it->free && it->start == address)
        {
            int id = it->id;
            free_block(id);
            return;
        }
    }
    cout << "No allocated block starts at address 0x"
         << hex << address << dec << endl;
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

void stats()
{
    size_t used =0;
    size_t free_mem = 0;
    size_t largest_free =0;
    size_t internal_frag =0;

    for(auto &b :memory_blocks)
    {
        if(b.free)
        {
            free_mem += b.size;
            if(b.size > largest_free)
                largest_free = b.size;
        }
        else
        {
            used += b.size;
            internal_frag += (b.size - b.requested);
        }
    }
    double utilization = (double)used / TOTAL_MEMORY * 100;
    double external_frag = (free_mem == 0) ? 0 :
        (1.0 - (double)largest_free / free_mem) * 100;

    cout << "\n--- Memory Statistics ---\n";
    cout << "Total memory: " << TOTAL_MEMORY << "\n";
    cout << "Used memory: " << used << "\n";
    cout << "Free memory: " << free_mem << "\n";
    cout << "Memory utilization: " << utilization << "%\n";
    cout << "Internal fragmentation: " << internal_frag << "\n";
    cout << "External fragmentation: " << external_frag << "%\n";
    cout << "Allocation success rate: "
         << alloc_success << "/" << alloc_requests << "\n";
}