#ifndef VM_H
#define VM_H

#include <vector>
#include <cstddef>

struct PageTableEntry {
    bool valid;
    int frame;
    int fifo_order;
};

class VirtualMemory {
public:
    int page_size;
    int num_frames;

    std::vector<PageTableEntry> page_table;
    std::vector<int> frame_to_page;

    int fifo_tick = 0;
    int page_faults = 0;
    int page_hits = 0;

    VirtualMemory(int frames, int page_size);

    size_t translate(size_t virtual_address);
    void stats();
};

#endif
