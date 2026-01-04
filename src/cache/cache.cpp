#include "cache.h"
#include <iostream>

using namespace std;

/* ============================================================
 * CacheLevel
 * ============================================================
 *
 * Represents a single cache level (e.g., L1, L2, L3).
 * Each cache is modeled as a set-associative cache with:
 *  - Fixed block size
 *  - Configurable associativity
 *  - FIFO or LRU replacement policy
 */

// Construct a cache level with given size, block size, associativity, and policy
CacheLevel::CacheLevel(int s, int b, int a, CachePolicy p)
    : size(s), block_size(b), associativity(a), policy(p)
{
    // Number of sets = total cache size / (block size × associativity)
    sets_count = size / (block_size * associativity);

    // Initialize cache sets and lines
    sets.resize(sets_count, vector<CacheLine>(associativity));

    // Initialize all cache lines as invalid
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

/*
 * Access a memory address in this cache level.
 *
 * @param address Physical memory address
 * @return true if cache hit, false if cache miss
 *
 * Implements:
 *  - Set indexing using modulo mapping
 *  - Tag comparison
 *  - FIFO or LRU replacement on miss
 */
bool CacheLevel::access(size_t address)
{
    // Compute block address, set index, and tag
    size_t block_addr = address / block_size;
    size_t set_index = block_addr % sets_count;
    size_t tag = block_addr / sets_count;

    auto &set = sets[set_index];

    // -------- Cache HIT check --------
    for (auto &line : set)
    {
        if (line.valid && line.tag == tag)
        {
            hits++;

            // Update access metadata for LRU
            line.last_used = ++fifo_tick;
            return true;
        }
    }

    // -------- Cache MISS handling --------
    misses++;
    fifo_tick++;

    CacheLine *victim = nullptr;

    // Select victim line based on replacement policy
    for (auto &line : set)
    {
        // Prefer empty (invalid) lines first
        if (!line.valid)
        {
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

    // Replace victim line
    victim->valid = true;
    victim->tag = tag;
    victim->fifo_counter = fifo_tick;
    victim->last_used = fifo_tick;

    return false;
}

/* ============================================================
 * CacheSystem
 * ============================================================
 *
 * Represents the full cache hierarchy:
 *  - L1: small, fast, LRU replacement
 *  - L2: larger, FIFO replacement
 *  - L3: largest, FIFO replacement (optional)
 */

// Construct cache hierarchy with configurable parameters
CacheSystem::CacheSystem(int l1_size, int l1_assoc,
                         int l2_size, int l2_assoc,
                         int l3_size, int l3_assoc)
    : L1(l1_size, 16, l1_assoc, LRU),
      L2(l2_size, 16, l2_assoc, FIFO),
      L3(l3_size, 16, l3_assoc, FIFO)
{}

/*
 * Access the cache hierarchy.
 *
 * Lookup order:
 *  L1 → L2 → L3 → Main Memory (simulated)
 *
 * Cache hits terminate the search early.
 */
void CacheSystem::access(size_t address)
{
    // Attempt access in L1 cache
    if (L1.access(address))
        return;

    // On L1 miss, attempt L2 cache
    if (L2.access(address))
        return;

    // On L1 and L2 miss, access L3 (or main memory)
    L3.access(address);
}

/*
 * Print cache statistics for each cache level.
 * Reports hits, misses, and hit ratio.
 */
void CacheSystem::stats()
{
    cout << "\n--- Cache Statistics ---\n";
    cout << "L1 Hits: " << L1.hits
         << " Misses: " << L1.misses
         << " Hit Ratio: " << L1.hit_ratio() << endl;

    cout << "L2 Hits: " << L2.hits
         << " Misses: " << L2.misses
         << " Hit Ratio: " << L2.hit_ratio() << endl;

    cout << "L3 Hits: " << L3.hits
         << " Misses: " << L3.misses
         << " Hit Ratio: " << L3.hit_ratio() << endl;
}

/*
 * Reinitialize the cache hierarchy at runtime.
 * This allows dynamic reconfiguration of cache sizes and associativity.
 */
void CacheSystem::reinit(int l1_size, int l1_assoc,
                         int l2_size, int l2_assoc,
                         int l3_size, int l3_assoc)
{
    L1 = CacheLevel(l1_size, 16, l1_assoc, LRU);
    L2 = CacheLevel(l2_size, 16, l2_assoc, FIFO);
    L3 = CacheLevel(l3_size, 16, l3_assoc, FIFO);

    cout << "Cache hierarchy reinitialized\n";
}
