#ifndef BUDDY_H
#define BUDDY_H

#include <map>
#include <vector>
#include <cstddef>

class BuddyAllocator {
public:
    size_t total_size;
    size_t min_block;

    std::map<size_t, std::vector<size_t>> free_lists;

    BuddyAllocator(size_t total, size_t min_block);

    size_t allocate(size_t size);
    void free_block(size_t addr, size_t size);

    void dump();
};

#endif
