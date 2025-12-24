#ifndef COMMON_H
#define COMMON_H
#include <cstddef>

struct Block {
    size_t start;
    size_t size;
    size_t requested;
    bool free;
    int id;
};

#endif