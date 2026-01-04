#include "vm.h"
#include <iostream>

using namespace std;

/*
 * Constructs a virtual memory subsystem.
 *
 * @param frames  Number of physical frames available
 * @param ps      Page size in bytes
 * @param p       Page replacement policy (FIFO or CLOCK)
 *
 * Initializes page tables, frame mappings, and replacement metadata.
 */
VirtualMemory::VirtualMemory(int frames, int ps, PageReplacement p)
    : num_frames(frames), page_size(ps), policy(p), clock_hand(0)
{
    // Fixed-size virtual address space: 256 virtual pages
    page_table.resize(256);

    // Frame-to-page reverse mapping (-1 indicates free frame)
    frame_to_page.resize(frames, -1);

    // Disk access counter used to simulate page fault cost
    disk_accesses = 0;

    // Initialize page table entries
    for (auto &pte : page_table)
    {
        pte.valid = false;
        pte.frame = -1;
        pte.fifo_order = 0;
        pte.reference = false;
    }
}

/*
 * Translates a virtual address to a physical address.
 *
 * Translation flow:
 * Virtual Address → Page Number + Offset → Page Table → Physical Frame
 *
 * Handles:
 * - Page hits
 * - Page faults
 * - Page replacement (FIFO or Clock)
 */
size_t VirtualMemory::translate(size_t virtual_address)
{
    size_t page = virtual_address / page_size;
    size_t offset = virtual_address % page_size;

    // Validate virtual page number
    if (page >= page_table.size())
    {
        cout << "Segmentation fault: invalid page access\n";
        return 0;
    }

    // Page hit: page already mapped to a physical frame
    if (page_table[page].valid)
    {
        page_hits++;
        page_table[page].reference = true;
        return page_table[page].frame * page_size + offset;
    }

    // PAGE FAULT
    page_faults++;
    disk_accesses++;
    fifo_tick++;

    int frame = -1;

    // Attempt to find a free physical frame
    for (int i = 0; i < num_frames; i++)
    {
        if (frame_to_page[i] == -1)
        {
            frame = i;
            break;
        }
    }

    // Clock (Second-Chance) replacement policy
    if (frame == -1 && policy == CLOCK_REPL)
    {
        while (true)
        {
            int victim_page = frame_to_page[clock_hand];

            // Select page with reference bit cleared
            if (!page_table[victim_page].reference)
            {
                frame = clock_hand;
                page_table[victim_page].valid = false;
                break;
            }

            // Clear reference bit and advance clock hand
            page_table[victim_page].reference = false;
            clock_hand = (clock_hand + 1) % num_frames;
        }
    }

    // FIFO page replacement policy
    if (frame == -1 && policy == FIFO_REPL)
    {
        int victim_page = -1;
        int oldest = 1e9;

        // Select the page with the oldest insertion timestamp
        for (int i = 0; i < 256; i++)
        {
            if (page_table[i].valid && page_table[i].fifo_order < oldest)
            {
                oldest = page_table[i].fifo_order;
                victim_page = i;
            }
        }

        frame = page_table[victim_page].frame;
        page_table[victim_page].valid = false;
    }

    // Map the new page into the selected frame
    page_table[page].valid = true;
    page_table[page].frame = frame;
    page_table[page].fifo_order = fifo_tick;
    page_table[page].reference = true;
    frame_to_page[frame] = page;

    return frame * page_size + offset;
}

/*
 * Returns the total size of the virtual address space in bytes.
 */
size_t VirtualMemory::get_virtual_memory_size() const
{
    return page_table.size() * page_size;
}

/*
 * Returns the configured page size in bytes.
 */
size_t VirtualMemory::get_page_size() const
{
    return page_size;
}

/*
 * Prints virtual memory statistics including page hits,
 * page faults, simulated disk accesses, and fault rate.
 */
void VirtualMemory::stats()
{
    cout << "\n--- Virtual Memory Stats ---\n";
    cout << "Page hits: " << page_hits << endl;
    cout << "Page faults: " << page_faults << endl;
    cout << "Disk accesses: " << disk_accesses << endl;
    cout << "Simulated disk latency per fault: "
         << DISK_LATENCY << " cycles" << endl;
    cout << "Page fault rate: " << fault_rate() << endl;
}
