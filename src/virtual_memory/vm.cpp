#include "vm.h"
#include <iostream>

using namespace std;

VirtualMemory::VirtualMemory(int frames, int ps)
    : num_frames(frames), page_size(ps)
{
    page_table.resize(256);
    frame_to_page.resize(frames, -1);

    for (auto &pte : page_table) {
        pte.valid = false;
        pte.frame = -1;
        pte.fifo_order = 0;
    }
}

size_t VirtualMemory::translate(size_t virtual_address)
{
    size_t page = virtual_address / page_size;
    size_t offset = virtual_address % page_size;

    if (page_table[page].valid) {
        page_hits++;
        return page_table[page].frame * page_size + offset;
    }

    // PAGE FAULT
    page_faults++;
    fifo_tick++;

    int frame = -1;

    // Find free frame
    for (int i = 0; i < num_frames; i++) {
        if (frame_to_page[i] == -1) {
            frame = i;
            break;
        }
    }

    // FIFO replacement
    if (frame == -1) {
        int victim_page = -1;
        int oldest = 1e9;

        for (int i = 0; i < 256; i++) {
            if (page_table[i].valid && page_table[i].fifo_order < oldest) {
                oldest = page_table[i].fifo_order;
                victim_page = i;
            }
        }

        frame = page_table[victim_page].frame;
        page_table[victim_page].valid = false;
    }

    page_table[page].valid = true;
    page_table[page].frame = frame;
    page_table[page].fifo_order = fifo_tick;
    frame_to_page[frame] = page;

    return frame * page_size + offset;
}

void VirtualMemory::stats()
{
    cout << "\n--- Virtual Memory Stats ---\n";
    cout << "Page hits: " << page_hits << endl;
    cout << "Page faults: " << page_faults << endl;
}
