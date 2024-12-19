#include "../inc/allocator_2.h"
#include <stdlib.h>
#include <string.h>

typedef struct Allocator {
    void* memory_start;
    size_t memory_size;
    size_t free_offset;
} Allocator;

Allocator* allocator_create(void* const memory, const size_t size) {
    Allocator* allocator = (Allocator*)malloc(sizeof(Allocator));
    allocator->memory_start = memory;
    allocator->memory_size = size;
    allocator->free_offset = 0;
    return allocator;
}

void allocator_destroy(Allocator* const allocator) {
    free(allocator);
}

void* allocator_alloc(Allocator* const allocator, const size_t size) {
    if (allocator->free_offset + size > allocator->memory_size) {
        return NULL;
    }
    void* block = (char*)allocator->memory_start + allocator->free_offset;
    allocator->free_offset += size;
    return block;
}

void allocator_free(Allocator* const allocator, void* const memory) {
    // No-op for simplicity
}