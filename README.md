# myMem
Replaces sbrk() with mmap() for Enhanced Efficiency 🚀

This C library provides a custom memory manager that implements malloc(), realloc(), free(), and calloc() functions, leveraging mmap() for superior performance compared to the traditional sbrk() approach.

## Key Features
✨ **Speed Boost**: mmap() enables memory allocation in larger chunks, reducing system calls and fragmentation, leading to faster memory allocation and deallocation.

📈 **Efficient Memory Management**: Provides a controlled environment for memory usage, optimizing performance for your applications.

🔄 **Coalesce Free Blocks**: The library includes a feature to coalesce adjacent free memory blocks, reducing fragmentation and improving memory reuse efficiency.

📊 **Memory Allocation Statistics**: Track and print memory allocation statistics, including total memory allocated, total memory freed, current memory usage, and allocation count.

## Functionality
**malloc()**
Allocates a block of memory of the specified size and returns a pointer to the beginning of the block.

**free()**
Frees a previously allocated block of memory, making it available for future allocations.

**calloc()**
Allocates memory for an array of elements, initializes all bytes to zero, and returns a pointer to the allocated space.

**realloc()**
Changes the size of a previously allocated memory block, copying the existing content to the new block.

**coalesce_free_blocks()**
Coalesces adjacent free memory blocks to reduce fragmentation and enhance memory reuse efficiency.

**print_memory_statistics()**
Prints detailed memory allocation statistics:

* **Total Allocated**: Total bytes allocated.
* **Total Freed**: Total bytes freed.
* **Current Usage**: Current bytes in use.
* **Allocation Count**: Number of allocation operations performed.

## Example Usage
```C
#include "myMem.h"

int main() {
    void *p1 = malloc(100);
    void *p2 = malloc(200);
    print_memory_statistics(); // Print statistics after allocations

    free(p1);
    free(p2);
    print_memory_statistics(); // Print statistics after frees

    return 0;
}
```

Installation
To use this custom memory manager in your project, include the header file and link the implementation file.
```
#include "myMem.h"
```
License
This project is licensed under the MIT License.
