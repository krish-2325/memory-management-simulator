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

int l1_size = 64, l1_assoc = 1;
int l2_size = 256, l2_assoc = 2;
int l3_size = 1024, l3_assoc = 4;

CacheSystem cache(l1_size, l1_assoc, l2_size, l2_assoc, l3_size, l3_assoc); // CacheSystem(L1_size, L1_assoc, L2_size, L2_assoc)
VirtualMemory vm(8,256,CLOCK_REPL);
BuddyAllocator buddy(1024, 16);
bool use_buddy = false;
int total_memory_accesses = 0;
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
        else if (command == "free") 
        {
                string arg;
                cin >> arg;

                if (arg.find("0x") == 0 || arg.find("0X") == 0) {
                    size_t address = stoul(arg, nullptr, 16);
                    free_by_address(address);
                } else {
                    int id = stoi(arg);
                    free_block(id);
                }
        }
        else if (command == "set")
        {
            string target;
            cin >> target;

            if (target == "cache")
            {
                string level;
                int size, assoc;
                cin >> level >> size >> assoc;

                if (level == "L1") {
                    l1_size = size;
                    l1_assoc = assoc;
                } else if (level == "L2") {
                    l2_size = size;
                    l2_assoc = assoc;
                } else if (level == "L3") {
                    l3_size = size;
                    l3_assoc = assoc;
                } else {
                    cout << "Unknown cache level\n";
                    continue;
                }

                cache.reinit(l1_size, l1_assoc, l2_size, l2_assoc, l3_size, l3_assoc);
            }
            else
            {
                // allocator selection
                if (target == "first_fit") {
                    use_buddy = false;
                    current_allocator = FIRST_FIT;
                }
                else if (target == "best_fit") {
                    use_buddy = false;
                    current_allocator = BEST_FIT;
                }
                else if (target == "worst_fit") {
                    use_buddy = false;
                    current_allocator = WORST_FIT;
                }
                else if (target == "buddy") {
                    use_buddy = true;
                }
                else {
                    cout << "Unknown set command\n";
                }
            }
        }
        else if(command=="stats")
        {
            stats();
        }
        else if(command == "access")
        {
            size_t vaddr;
            cin >> hex >> vaddr >> dec;
            size_t vmem_size = vm.get_virtual_memory_size();
            if (vaddr >= vmem_size)
            {
                cout << "Invalid virtual address 0x"
                    << hex << vaddr << dec
                    << " (out of range)\n";
                continue;
            }
            total_memory_accesses++;
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
        else if (command == "summary") 
        {
            cout << "\n--- System Summary ---\n";
            cout << "Total memory accesses: " << total_memory_accesses << endl;
            cache.stats();
            vm.stats();
        }
        else
        {
            cout<<"Unknown command"<<endl;
        }
    }
    return 0;
}