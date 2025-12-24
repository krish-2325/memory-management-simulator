#include "cache.h"
#include <iostream>

using namespace std;

// ---------- CacheLevel ----------

CacheLevel::CacheLevel(int s, int b, int a)
    : size(s), block_size(b), associativity(a)
{
    sets_count = size / (block_size * associativity);
    sets.resize(sets_count, vector<CacheLine>(associativity));

    for (auto &set : sets)
        for (auto &line : set)
            line.valid = false;
}

bool CacheLevel::access(size_t address)
{
    size_t block_addr = address / block_size;
    size_t set_index = block_addr % sets_count;
    size_t tag = block_addr / sets_count;

    auto &set = sets[set_index];

    // HIT check
    for (auto &line : set) {
        if (line.valid && line.tag == tag) {
            hits++;
            return true;
        }
    }

    // MISS → FIFO replacement
    misses++;
    fifo_tick++;

    CacheLine *victim = &set[0];
    for (auto &line : set) {
        if (!line.valid) {
            victim = &line;
            break;
        }
        if (line.fifo_counter < victim->fifo_counter)
            victim = &line;
    }

    victim->valid = true;
    victim->tag = tag;
    victim->fifo_counter = fifo_tick;

    return false;
}

// ---------- CacheSystem ----------

CacheSystem::CacheSystem()
    : L1(64, 16, 1),    // 64B cache, 16B block, direct mapped
      L2(256, 16, 2)    // 256B cache, 2-way
{}

void CacheSystem::access(size_t address)
{
    if (L1.access(address))
        return;

    if (L2.access(address))
        return;

    // Miss in both → main memory (simulated)
}

void CacheSystem::stats()
{
    cout << "\n--- Cache Statistics ---\n";
    cout << "L1 Hits: " << L1.hits << " Misses: " << L1.misses << endl;
    cout << "L2 Hits: " << L2.hits << " Misses: " << L2.misses << endl;
}
