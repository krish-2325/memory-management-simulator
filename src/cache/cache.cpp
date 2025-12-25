#include "cache.h"
#include <iostream>

using namespace std;

// ---------- CacheLevel ----------

CacheLevel::CacheLevel(int s, int b, int a, CachePolicy p)
    : size(s), block_size(b), associativity(a), policy(p)
{
    sets_count = size / (block_size * associativity);
    sets.resize(sets_count, vector<CacheLine>(associativity));

    for (auto &set : sets)
    {
        for (auto &line : set)
        {
            line.valid = false;
            line.fifo_counter = 0;
            line.last_used = 0;
        }
    }    
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
            line.last_used = fifo_tick; // For LRU
            return true;
        }
    }

    // MISS → FIFO replacement
    misses++;
    fifo_tick++;

    CacheLine *victim = nullptr;

    for (auto &line : set) {
        if (!line.valid) {
            victim = &line;
            break;
        }

        if (!victim)
            victim = &line;
        else if (policy == FIFO && line.fifo_counter < victim->fifo_counter)
            victim = &line;
        else if (policy == LRU && line.last_used < victim->last_used)
            victim = &line;
    }

    victim->valid = true;
    victim->tag = tag;
    victim->fifo_counter = fifo_tick;
    victim->last_used = fifo_tick;

    return false;
}

// ---------- CacheSystem ----------

CacheSystem::CacheSystem(int l1_size, int l1_assoc, int l2_size, int l2_assoc)
    : L1(l1_size, 16, l1_assoc, LRU),
      L2(l2_size, 16, l2_assoc, FIFO)
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
