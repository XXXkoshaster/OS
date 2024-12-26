#ifndef ALLOCATOR2_H
#define ALLOCATOR2_H

#include <stddef.h>

#define NUM_FREE_LISTS 20

#define NDX(size) \
    ((size) > 128 \
        ? (size) > 256 ? 4 : 3 \
        : (size) > 64 \
            ? 2 \
            : (size) > 32 ? 1 : 0)

struct FreeBlock {
    size_t size;
    struct FreeBlock* next;
};

struct Allocator {
    void* base_memory;
    size_t memory_size;
    struct FreeBlock* freelists[NUM_FREE_LISTS + 1];
};

struct Allocator* allocator_create(void* memory, size_t size);
void allocator_destroy(struct Allocator* allocator);
void* allocator_alloc(struct Allocator* allocator, size_t size);
void allocator_free(struct Allocator* allocator, void* memory);

#endif // ALLOCATOR2_H