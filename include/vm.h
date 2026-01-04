#ifndef VM_H
#define VM_H

#include <vector>
#include <cstddef>

/*
 * Page Table Entry (PTE)
 *
 * Represents a single virtual page's metadata used for
 * address translation and page replacement decisions.
 */
struct PageTableEntry {
    bool valid;          // Indicates whether the page is currently resident in physical memory
    int frame;           // Physical frame number mapped to this page
    int fifo_order;      // Timestamp for FIFO page replacement
    bool reference;      // Reference bit used by Clock replacement algorithm
};

/*
 * Supported page replacement policies.
 */
enum PageReplacement {
    FIFO_REPL,           // First-In First-Out replacement
    CLOCK_REPL           // Clock (Second-Chance) replacement
};

/*
 * VirtualMemory
 *
 * Simulates a virtual memory subsystem using paging.
 * Responsible for:
 *  - Translating virtual addresses to physical addresses
 *  - Handling page faults
 *  - Applying page replacement policies
 *  - Collecting virtual memory statistics
 */
class VirtualMemory {
public:
    /*
     * Configuration parameters
     */
    int page_size;        // Size of each page in bytes
    int num_frames;       // Number of physical frames available
    PageReplacement policy; // Selected page replacement policy
    int clock_hand;       // Pointer used for Clock replacement

    /*
     * Disk simulation parameters
     */
    int disk_accesses;          // Number of simulated disk accesses (page faults)
    const int DISK_LATENCY = 100; // Symbolic disk access latency (cycles)

    /*
     * Core data structures
     */
    std::vector<PageTableEntry> page_table; // Page table indexed by virtual page number
    std::vector<int> frame_to_page;         // Reverse mapping: frame → page

    /*
     * Statistics counters
     */
    int fifo_tick = 0;     // Global time counter for FIFO ordering
    int page_faults = 0;   // Total number of page faults
    int page_hits = 0;     // Total number of page hits

    /*
     * Constructor
     *
     * Initializes the virtual memory system with:
     *  - A fixed number of frames
     *  - A fixed page size
     *  - A selected page replacement policy
     */
    VirtualMemory(int frames, int page_size, PageReplacement policy);

    /*
     * Compute page fault rate as a percentage.
     */
    double fault_rate() const 
    {
        int total = page_hits + page_faults;
        return total == 0 ? 0.0
                          : (double)page_faults / total * 100.0;
    }

    /*
     * Translate a virtual address to a physical address.
     *
     * Steps:
     *  - Extract page number and offset
     *  - Check page table validity
     *  - Handle page faults if necessary
     *  - Apply replacement policy if frames are full
     */
    size_t translate(size_t virtual_address);

    /*
     * Returns total virtual memory size in bytes.
     */
    size_t get_virtual_memory_size() const;

    /*
     * Returns page size in bytes.
     */
    size_t get_page_size() const;

    /*
     * Print virtual memory statistics including:
     *  - Page hits
     *  - Page faults
     *  - Disk accesses
     *  - Fault rate
     */
    void stats();
};

#endif
