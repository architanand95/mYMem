#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

// Define alignment for memory blocks
typedef char ALIGN[16];

// Global variables to track memory statistics
size_t total_allocated = 0;
size_t total_freed = 0;
size_t current_usage = 0;
size_t allocation_count = 0;

// Union to represent a memory block header
union header {
    struct {
        size_t size;          // Size of the memory block
        unsigned is_free;     // Flag to indicate if the block is free
        union header *next;   // Pointer to the next block in the list
    } s;
    ALIGN stub;              // Aligning the header to 16 bytes
};

typedef union header header_t;

// Pointers to the start and end of the memory block list
header_t *head = NULL, *tail = NULL;

// Mutex for thread-safe operations
pthread_mutex_t global_malloc_lock;


// Function to find a free memory block of at least 'size' bytes
header_t *get_free_block(size_t size) {
    header_t *curr = head;
    size_t tempSize = __INT_MAX__;
    while (curr) {
        if (curr->s.is_free && curr->s.size >= size)
        {
            if(tempSize > curr->s.size)
                tempSize = curr->s.size;
        }
        curr = curr->s.next;
    }
    header_t *curr1 = head;
    while(curr1){
        if(curr1->s.is_free && curr1->s.size == tempSize)
            return curr1;
        curr1 = curr1->s.next;
    }
    return NULL;
}

// Custom malloc function
void *malloc(size_t size) {
    size_t total_size;
    void *block;
    header_t *header;
    if (!size)
        return NULL;
    pthread_mutex_lock(&global_malloc_lock);
    header = get_free_block(size);
    if (header) {
        header->s.is_free = 0;
        pthread_mutex_unlock(&global_malloc_lock);
        current_usage += size;
        allocation_count++;
        return (void *)(header + 1);
    }
    total_size = sizeof(header_t) + size;
    block = sbrk(total_size);
    if (block == (void *)-1) {
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL;
    }
    header = block;
    header->s.size = size;
    header->s.is_free = 0;
    header->s.next = NULL;
    if (!head)
        head = header;
    if (tail)
        tail->s.next = header;
    tail = header; 
    pthread_mutex_unlock(&global_malloc_lock);
    total_allocated += size;
    current_usage += size;
    allocation_count++;
    return (void *)(header + 1);
}

// Function to coalesce adjacent free memory blocks
void coalesce_free_blocks() {
    header_t *curr = head;
    while (curr && curr->s.next) {
        if (curr->s.is_free && curr->s.next->s.is_free) {
            curr->s.size += sizeof(header_t) + curr->s.next->s.size;
            curr->s.next = curr->s.next->s.next;
            if (curr->s.next == NULL)
                tail = curr;
        } else {
            curr = curr->s.next;
        }
    }
}

// Custom free function
void free(void *block) {
    header_t *header, *tmp;
    void *programbreak;
    if (!block)
        return;
    pthread_mutex_lock(&global_malloc_lock);
    header = (header_t *)block - 1;
    programbreak = sbrk(0);
    if ((char *)block + header->s.size == programbreak) {
        if (head == tail)
            head = tail = NULL;
        else {
            tmp = head;
            while (tmp) {
                if (tmp->s.next == tail) {
                    tmp->s.next = NULL;
                    tail = tmp;
                }
                tmp = tmp->s.next;
            }
        }
        sbrk(0 - sizeof(header_t) - header->s.size);
        pthread_mutex_unlock(&global_malloc_lock);
        total_freed += header->s.size;
        current_usage -= header->s.size;
        return;
    }
    header->s.is_free = 1;
    coalesce_free_blocks();
    total_freed += header->s.size;
    current_usage -= header->s.size;
    pthread_mutex_unlock(&global_malloc_lock);
}

// Custom calloc function
void *calloc(size_t num, size_t nsize) {
    if (!num || !nsize)
        return NULL;
    size_t size;
    void *block;
    size = num * nsize;
    if (nsize != size / num)
        return NULL;
    block = malloc(size);
    if (!block)
        return NULL;
    memset(block, 0, size);
    return block;
}

// Custom realloc function
void *realloc(void *block, size_t size) {
    void *ret;
    header_t *header;
    if (!block)
        return malloc(size);
    if (!size) {
        free(block);
        return NULL;
    }
    header = (header_t *)block - 1;
    if (header->s.size >= size)
        return block;
    ret = malloc(size);
    if (ret) {
        memcpy(ret, block, header->s.size);
        free(block);
    }
    return ret;
}

// Function to print memory allocation statistics
void print_memory_statistics() {
    pthread_mutex_lock(&global_malloc_lock);
    printf("Memory Allocation Statistics:\n");
    printf("Total Allocated: %zu bytes\n", total_allocated);
    printf("Total Freed: %zu bytes\n", total_freed);
    printf("Current Usage: %zu bytes\n", current_usage);
    pthread_mutex_unlock(&global_malloc_lock);
}
