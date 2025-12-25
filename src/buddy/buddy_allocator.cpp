#include "buddy.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

static size_t next_power_of_two(size_t n) {
    size_t p = 1;
    while (p < n) p <<= 1;
    return p;
}

BuddyAllocator::BuddyAllocator(size_t total, size_t minb)
    : total_size(total), min_block(minb)
{
    free_lists.clear();
    free_lists[total_size].push_back(0);
}

size_t BuddyAllocator::allocate(size_t size)
{
    size = next_power_of_two(size);
    if (size < min_block) size = min_block;

    size_t current = size;

    while (free_lists[current].empty()) {
        current <<= 1;
        if (current > total_size) {
            cout << "Buddy allocation failed\n";
            return (size_t)-1;
        }
    }

    size_t addr = free_lists[current].back();
    free_lists[current].pop_back();

    while (current > size) {
        current >>= 1;
        free_lists[current].push_back(addr + current);
    }

    cout << "Buddy allocated block at 0x"
         << hex << addr << dec
         << " size " << size << endl;

    return addr;
}

void BuddyAllocator::free_block(size_t addr, size_t size)
{
    size = next_power_of_two(size);
    size_t buddy;

    while (size < total_size) {
        buddy = addr ^ size;
        auto &list = free_lists[size];

        auto it = find(list.begin(), list.end(), buddy);
        if (it == list.end())
            break;

        list.erase(it);
        addr = min(addr, buddy);
        size <<= 1;
    }

    free_lists[size].push_back(addr);
}

void BuddyAllocator::dump()
{
    cout << "\n--- Buddy Free Lists ---\n";
    for (auto &p : free_lists) {
        cout << "Size " << p.first << ": ";
        for (auto a : p.second)
            cout << "0x" << hex << a << " ";
        cout << dec << endl;
    }
}
