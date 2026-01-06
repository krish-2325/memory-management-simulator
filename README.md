# Memory Management Simulator

## Overview
This project implements a **user-space Memory Management Simulator** that models
key operating system concepts such as dynamic memory allocation, CPU cache
hierarchies, and virtual memory using paging.

The simulator focuses on **algorithmic correctness**, **systems-level design**,
and **performance trade-offs**, rather than low-level hardware or kernel
implementation.

---

## Features

### 1. Physical Memory Simulation
- Contiguous, byte-addressable physical memory
- Configurable memory size
- Explicit tracking of allocated and free blocks
- Memory dump visualization
- Internal and external fragmentation statistics

### 2. Memory Allocation Strategies
- First Fit
- Best Fit
- Worst Fit
- Buddy Allocation (optional extension)

Supports:
- Block splitting on allocation
- Coalescing adjacent free blocks on deallocation
- Freeing memory by **block ID or starting address**

### 3. Cache Simulation
- Multilevel cache hierarchy:
  - L1 Cache (LRU replacement)
  - L2 Cache (FIFO replacement)
  - Optional L3 Cache (FIFO replacement)
- Configurable cache sizes and associativity at runtime
- Cache hit, miss, and hit-ratio statistics
- Proper miss propagation across cache levels

### 4. Virtual Memory Simulation
- Paging-based virtual memory model
- Page tables with valid bits and frame mapping
- FIFO and Clock page replacement policies
- Page hit and page fault tracking
- Virtual address bounds checking

---

## Project Structure
```
memory-simulator/
├── src/
│   ├── allocator/
│   ├── cache/
│   ├── virtual_memory/
│   ├── buddy/
│   └── main.cpp
├── include/
├── tests/
├── logs/
├── docs/
├── README.md
```

---

## Build Instructions

### Windows (PowerShell)

Ensure `g++` (MinGW or similar) is available in PATH.

```powershell
g++ src/main.cpp src/allocator/allocator.cpp src/allocator/first_fit.cpp `
   src/allocator/best_fit.cpp src/allocator/worst_fit.cpp `
   src/cache/cache.cpp src/virtual_memory/vm.cpp `
   src/buddy/buddy_allocator.cpp -Iinclude -o memsim
```

Run the simulator:
```powershell
.\memsim.exe
```

---

### Linux / macOS

```bash
g++ src/main.cpp src/allocator/allocator.cpp src/allocator/first_fit.cpp \
   src/allocator/best_fit.cpp src/allocator/worst_fit.cpp \
   src/cache/cache.cpp src/virtual_memory/vm.cpp \
   src/buddy/buddy_allocator.cpp -Iinclude -o memsim
```

Run the simulator:
```bash
./memsim
```

---

## Usage

### Initialize Memory
```
init memory 1024
```

### Set Allocation Strategy
```
set first_fit
set best_fit
set worst_fit
set buddy
```

### Allocate and Free Memory
```
malloc 100
free 1
free 0x0000
```

### Dump Memory Layout
```
dump
```

### Memory Statistics
```
stats
```

---

## Cache Commands

### Access Memory (Virtual → Physical → Cache)
```
access 0x0100
```

### Cache Statistics
```
cache_stats
```

### Configure Cache at Runtime
```
set cache L1 128 2
set cache L2 512 4
set cache L3 1024 4
```

---

## Virtual Memory Commands

### Virtual Memory Statistics
```
vm_stats
```

Invalid virtual addresses are detected and rejected.

---

## Buddy Allocator Commands
```
set buddy
malloc 100
malloc 200
buddy_dump
buddy_free 0x0000 128
```

---

## Running Test Workloads

### Windows (PowerShell)

```powershell
Get-Content tests\allocator_test.txt | .\memsim.exe > logs\allocator.log
Get-Content tests\cache_test.txt | .\memsim.exe > logs\cache.log
Get-Content tests\vm_test.txt | .\memsim.exe > logs\vm.log
Get-Content tests\buddy_test.txt | .\memsim.exe > logs\buddy.log
```

### Linux / macOS

```bash
./memsim < tests/allocator_test.txt > logs/allocator.log
./memsim < tests/cache_test.txt > logs/cache.log
./memsim < tests/vm_test.txt > logs/vm.log
./memsim < tests/buddy_test.txt > logs/buddy.log
```

Each test file ends with the `exit` command to terminate the simulator cleanly.

---

## Documentation
- Detailed design explanation: `docs/design.md`
- Test outputs and demonstration logs: `logs/`

---

## Demo Video
- https://drive.google.com/file/d/1RUmLKB4anCtkuH7xtBVgzpIG7t5mbBjK/view?usp=drive_link

---

## Assumptions and Limitations
- All memory units are expressed in **bytes**
- Disk access latency is symbolic
- Single-process simulation
- No real concurrency or scheduling

---

## Summary
This simulator provides a modular and extensible framework to understand
operating system memory management concepts, emphasizing correctness,
clarity, and system-level reasoning.
