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
             cout<<"Free ";
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
        else if (command == "malloc") {
            size_t size;
            cin >> size;
            malloc_first_fit(size);
        }
        else
        {
            cout<<"Unknown command: "<<endl;
        }
    }
    return 0;
}