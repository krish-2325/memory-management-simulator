#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <cstddef>

/*
 * CacheLine
 *
 * Represents a single cache line within a cache set.
 * Stores metadata required for cache lookup and replacement.
 */
struct CacheLine {
    bool valid;          // Indicates whether the cache line contains valid data
    size_t tag;          // Tag used for address matching
    int fifo_counter;    // Timestamp used for FIFO replacement
    int last_used;       // Timestamp used for LRU replacement
};

/*
 * Supported cache replacement policies.
 */
enum CachePolicy {
    FIFO,   // First-In First-Out replacement
    LRU     // Least Recently Used replacement
};

/*
 * CacheLevel
 *
 * Models a single cache level (e.g., L1, L2, L3).
 * Implements set-associative cache mapping with configurable
 * size, block size, associativity, and replacement policy.
 */
class CacheLevel {
public:
    /*
     * Cache configuration parameters
     */
    int size;           // Total cache size in bytes
    int block_size;     // Cache block size in bytes
    int associativity;  // Number of cache lines per set

    /*
     * Derived parameters and storage
     */
    int sets_count;     // Number of sets in the cache
    std::vector<std::vector<CacheLine>> sets; // Cache sets

    /*
     * Statistics counters
     */
    int hits = 0;       // Number of cache hits
    int misses = 0;     // Number of cache misses
    int fifo_tick = 0;  // Global timestamp for replacement tracking

    CachePolicy policy; // Replacement policy for this cache level

    /*
     * Constructor
     *
     * Initializes a cache level with the given parameters.
     */
    CacheLevel(int size, int block_size, int associativity, CachePolicy policy);

    /*
     * Access a memory address in this cache level.
     *
     * Returns true on cache hit, false on cache miss.
     * On miss, performs replacement according to the
     * configured replacement policy.
     */
    bool access(size_t address);

    /*
     * Compute hit ratio as a percentage.
     */
    double hit_ratio() const
    {
        int total = hits + misses;
        return total == 0 ? 0.0
                          : (double)hits / total * 100.0;
    }
};

/*
 * CacheSystem
 *
 * Models a multilevel cache hierarchy.
 * Currently supports:
 *  - L1 Cache
 *  - L2 Cache
 *  - L3 Cache
 *
 * Cache access follows the hierarchy:
 * L1 → L2 → L3 → Main Memory
 */
class CacheSystem {
public:
    CacheLevel L1;   // Level 1 cache (fastest, smallest)
    CacheLevel L2;   // Level 2 cache
    CacheLevel L3;   // Level 3 cache (largest)

    /*
     * Constructor
     *
     * Initializes the cache hierarchy with configurable
     * sizes and associativities for each level.
     */
    CacheSystem(int l1_size, int l1_assoc,
                int l2_size, int l2_assoc,
                int l3_size, int l3_assoc);

    /*
     * Access a physical memory address through the cache hierarchy.
     */
    void access(size_t address);

    /*
     * Reinitialize the cache hierarchy with new parameters.
     * Existing cache contents are discarded.
     */
    void reinit(int l1_size, int l1_assoc,
                int l2_size, int l2_assoc,
                int l3_size, int l3_assoc);

    /*
     * Print cache statistics for all levels.
     */
    void stats();
};

#endif
