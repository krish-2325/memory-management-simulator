#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <cstddef>

struct CacheLine {
    bool valid;
    size_t tag;
    int fifo_counter;
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

    CacheLevel(int size, int block_size, int associativity);

    bool access(size_t address);
};

class CacheSystem {
public:
    CacheLevel L1;
    CacheLevel L2;

    CacheSystem();

    void access(size_t address);
    void stats();
};

#endif
