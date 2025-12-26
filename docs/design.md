# Design Document: Memory Management Simulator

## 1. Introduction
This document describes the design and implementation of the **Memory Management Simulator**.
The simulator models key operating system memory-management responsibilities including
dynamic memory allocation, cache management, and virtual memory using paging.

The design prioritizes correctness, modularity, and clarity over hardware-specific optimizations.

---

## 2. Assumptions and Memory Model

### Memory Units
- All memory sizes, addresses, and offsets are expressed in **bytes**.
- Physical memory is modeled as a contiguous, byte-addressable region.

### Scope
- Single-process simulation
- No real hardware timing
- No concurrency or scheduling

---

## 3. Physical Memory Layout

Physical memory is represented as a linked list of memory blocks.

Each block contains:
- Start address
- Block size
- Allocation status (free/allocated)
- Requested size (for fragmentation calculation)
- Block ID (for allocated blocks)

Adjacent free blocks are coalesced on deallocation to reduce fragmentation.

---

## 4. Dynamic Memory Allocation Strategies

### 4.1 First Fit
Allocates the first free block large enough to satisfy the request.
This strategy is fast but can cause external fragmentation.

### 4.2 Best Fit
Selects the smallest free block that satisfies the request.
This reduces wasted space but increases search time.

### 4.3 Worst Fit
Selects the largest available free block.
This aims to leave medium-sized free blocks for future requests.

### 4.4 Buddy Allocation (Optional)
- Total memory size is a power of two
- Allocation requests are rounded up to the nearest power of two
- Blocks are split recursively
- Buddy blocks are merged on free using XOR-based address computation
- Free lists are maintained per block size

---

## 5. Fragmentation Metrics and Statistics

The simulator tracks:
- **Internal fragmentation**: unused memory within allocated blocks
- **External fragmentation**: free memory that cannot be allocated as one contiguous block
- Allocation success and failure rate
- Overall memory utilization

These metrics are reported through the `stats` command.

---

## 6. Cache Simulation Design

### 6.1 Cache Hierarchy
The simulator models a multilevel cache hierarchy:
- **L1 Cache**: Small, fast cache using LRU replacement
- **L2 Cache**: Larger cache using FIFO replacement
- **L3 Cache (optional)**: Largest cache using FIFO replacement

### 6.2 Cache Parameters
Each cache level has:
- Configurable size
- Fixed block size (16 bytes)
- Configurable associativity
- Replacement policy (FIFO or LRU)

Cache parameters can be reconfigured at runtime via CLI commands.

### 6.3 Cache Access Flow
1. Address lookup in L1 cache
2. On miss, lookup in L2 cache
3. On miss, lookup in L3 cache
4. On miss, access main memory

Each level tracks hit and miss statistics independently.

---

## 7. Virtual Memory Design

### 7.1 Virtual Address Space
- Page size: **256 bytes**
- Number of virtual pages: **256**
- Virtual address space size: **64 KB**
- Virtual addresses are validated before translation

### 7.2 Page Table
Each page table entry contains:
- Valid bit
- Physical frame number
- FIFO timestamp
- Reference bit (for Clock replacement)

### 7.3 Address Translation
Virtual address translation follows:
Virtual Address → Page Number + Offset → Page Table → Physical Frame → Physical Address

Invalid virtual addresses are rejected before translation.

---

## 8. Page Replacement Policies

### FIFO
- Evicts the page with the oldest insertion time

### Clock
- Uses a circular buffer and reference bits
- Approximates LRU behavior with lower overhead

Page faults and page hits are tracked and reported.

---

## 9. Integration Between Components

The simulator integrates components in the following order:
Virtual Address → Page Table → Physical Address → Cache Hierarchy → Main Memory

This mirrors real operating system memory access pipelines.

---

## 10. Testing and Demonstration

Test workloads are provided in the `tests/` directory:
- Allocator tests
- Cache behavior tests
- Virtual memory tests
- Buddy allocator tests

Execution logs are stored in the `logs/` directory and serve as demonstration artifacts.

---

## 11. Limitations

- No simulation of real disk I/O latency
- No multi-process support
- No concurrency or scheduling
- Cache coherence is not modeled

---

## 12. Conclusion

This simulator provides a clear and modular framework for understanding operating system
memory management concepts. It demonstrates allocation strategies, caching behavior,
virtual memory translation, and fragmentation trade-offs in a controlled user-space
environment.
