#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <cstddef>

struct CacheLine {
    bool valid;
    size_t tag;
    int fifo_counter;
    int last_used;
};

enum CachePolicy {
    FIFO,
    LRU
};

class CacheLevel {
public:
    int size;           // total cache size in bytes
    int block_size;     // block size in bytes
    int associativity;  // lines per set

    int sets_count;
    std::vector<std::vector<CacheLine>> sets;

    int hits = 0;
    int misses = 0;
    int fifo_tick = 0;
    CachePolicy policy;

    CacheLevel(int size, int block_size, int associativity, CachePolicy policy);

    bool access(size_t address);
    double hit_ratio() const 
    {
        int total = hits + misses;
        return total == 0 ? 0.0 : (double)hits / total * 100.0;
    }
};

class CacheSystem {
public:
    CacheLevel L1;
    CacheLevel L2;
    CacheLevel L3;

    CacheSystem(int l1_size, int l1_assoc, int l2_size, int l2_assoc, int l3_size, int l3_assoc);

    void access(size_t address);
    void stats();
};

#endif
