#include <iostream>
#include <list>
#include <iomanip>
#include <string>
#include "common.h"
#include "cache.h"
#include "vm.h"
#include "buddy.h"
#include "allocator.h"

using namespace std;

CacheSystem cache;
VirtualMemory vm(8,256);
BuddyAllocator buddy(1024, 16);
bool use_buddy = false;
AllocatorType current_allocator = FIRST_FIT;

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
            init_memory(size);
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
            if (use_buddy) {
                buddy.allocate(size);
            } else {
                if (current_allocator == FIRST_FIT)
                    malloc_first_fit(size);
                else if (current_allocator == BEST_FIT)
                    malloc_best_fit(size);
                else
                    malloc_worst_fit(size);
            }
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
            if (type == "first_fit") 
            {
                use_buddy = false;
                current_allocator = FIRST_FIT;
            }
            else if (type == "best_fit") \
            {
                use_buddy = false;
                current_allocator = BEST_FIT;
            }
            else if (type == "worst_fit") 
            {
                use_buddy = false;
                current_allocator = WORST_FIT;
            }
            else if (type == "buddy") 
            {
                use_buddy = true;
            }
            else {
                cout << "Unknown allocator type\n";
            }
            cout<<"Allocator set to "<<type<<endl;
        }
        else if(command=="stats")
        {
            stats();
        }
        else if(command == "access")
        {
            size_t vaddr;
            cin >> hex >> vaddr >> dec;
            size_t paddr = vm.translate(vaddr);
            cache.access(paddr);
        }
        else if(command=="cache_stats")
        {
            cache.stats();
        }
        else if(command=="vm_stats")
        {
            vm.stats();
        }
        else if (command == "buddy_dump") 
        {
            buddy.dump();
        }
        else if (command == "buddy_free") 
        {
            size_t addr, size;
            cin >> hex >> addr >> dec >> size;
            buddy.free_block(addr, size);
        }
        else
        {
            cout<<"Unknown command"<<endl;
        }
    }
    return 0;
}