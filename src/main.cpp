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

/*
 * Default cache configuration parameters.
 * These can be modified at runtime using the CLI.
 */
int l1_size = 64,  l1_assoc = 1;
int l2_size = 256, l2_assoc = 2;
int l3_size = 1024, l3_assoc = 4;

/*
 * Core simulator components.
 * - CacheSystem models the multilevel cache hierarchy
 * - VirtualMemory models paging and address translation
 * - BuddyAllocator models power-of-two memory allocation
 */
CacheSystem cache(l1_size, l1_assoc,
                  l2_size, l2_assoc,
                  l3_size, l3_assoc);

VirtualMemory vm(8, 256, CLOCK_REPL);
BuddyAllocator buddy(1024, 16);

/*
 * Global simulator state flags and counters.
 */
bool use_buddy = false;
int total_memory_accesses = 0;
AllocatorType current_allocator = FIRST_FIT;

/*
 * Entry point of the memory management simulator.
 *
 * Implements an interactive command-line interface (CLI)
 * that allows users to issue memory allocation, deallocation,
 * cache access, and virtual memory commands.
 */
int main()
{
    string command;

    cout << "Memory Simulator Started\n";

    // Main command-processing loop
    while (true)
    {
        cout << "> ";
        cin >> command;

        /*
         * Initialize physical memory.
         * Usage: init memory <size>
         */
        if (command == "init")
        {
            string temp;
            size_t size;
            cin >> temp >> size;
            init_memory(size);
        }

        /*
         * Dump the current physical memory layout.
         */
        else if (command == "dump")
        {
            dump_memory();
        }

        /*
         * Terminate the simulator.
         */
        else if (command == "exit")
        {
            break;
        }

        /*
         * Allocate memory using the currently selected allocator.
         * Supports First Fit, Best Fit, Worst Fit, or Buddy allocation.
         */
        else if (command == "malloc")
        {
            size_t size;
            cin >> size;

            if (use_buddy)
            {
                buddy.allocate(size);
            }
            else
            {
                if (current_allocator == FIRST_FIT)
                    malloc_first_fit(size);
                else if (current_allocator == BEST_FIT)
                    malloc_best_fit(size);
                else
                    malloc_worst_fit(size);
            }
        }

        /*
         * Free memory by block ID or starting address.
         * Supports both:
         *   free <id>
         *   free 0x<address>
         */
        else if (command == "free")
        {
            string arg;
            cin >> arg;

            if (arg.find("0x") == 0 || arg.find("0X") == 0)
            {
                size_t address = stoul(arg, nullptr, 16);
                free_by_address(address);
            }
            else
            {
                int id = stoi(arg);
                free_block(id);
            }
        }

        /*
         * Set allocator or cache configuration.
         *
         * Examples:
         *   set first_fit
         *   set buddy
         *   set cache L1 128 2
         */
        else if (command == "set")
        {
            string target;
            cin >> target;

            // Cache reconfiguration
            if (target == "cache")
            {
                string level;
                int size, assoc;
                cin >> level >> size >> assoc;

                if (level == "L1")
                {
                    l1_size = size;
                    l1_assoc = assoc;
                }
                else if (level == "L2")
                {
                    l2_size = size;
                    l2_assoc = assoc;
                }
                else if (level == "L3")
                {
                    l3_size = size;
                    l3_assoc = assoc;
                }
                else
                {
                    cout << "Unknown cache level\n";
                    continue;
                }

                cache.reinit(l1_size, l1_assoc,
                             l2_size, l2_assoc,
                             l3_size, l3_assoc);
            }
            // Allocator selection
            else
            {
                if (target == "first_fit")
                {
                    use_buddy = false;
                    current_allocator = FIRST_FIT;
                }
                else if (target == "best_fit")
                {
                    use_buddy = false;
                    current_allocator = BEST_FIT;
                }
                else if (target == "worst_fit")
                {
                    use_buddy = false;
                    current_allocator = WORST_FIT;
                }
                else if (target == "buddy")
                {
                    use_buddy = true;
                }
                else
                {
                    cout << "Unknown set command\n";
                }
            }
        }

        /*
         * Print physical memory statistics.
         */
        else if (command == "stats")
        {
            stats();
        }

        /*
         * Access a virtual address.
         * Flow:
         *   Virtual Address → Page Table → Physical Address → Cache Hierarchy
         */
        else if (command == "access")
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

        /*
         * Print cache hit/miss statistics for all levels.
         */
        else if (command == "cache_stats")
        {
            cache.stats();
        }

        /*
         * Print virtual memory statistics.
         */
        else if (command == "vm_stats")
        {
            vm.stats();
        }

        /*
         * Dump buddy allocator free lists.
         */
        else if (command == "buddy_dump")
        {
            buddy.dump();
        }

        /*
         * Free a buddy-allocated block.
         * Usage: buddy_free 0x<address> <size>
         */
        else if (command == "buddy_free")
        {
            size_t addr, size;
            cin >> hex >> addr >> dec >> size;
            buddy.free_block(addr, size);
        }

        /*
         * Print a combined system summary.
         */
        else if (command == "summary")
        {
            cout << "\n--- System Summary ---\n";
            cout << "Total memory accesses: "
                 << total_memory_accesses << endl;
            cache.stats();
            vm.stats();
        }

        /*
         * Handle unknown commands.
         */
        else
        {
            cout << "Unknown command\n";
        }
    }

    return 0;
}
