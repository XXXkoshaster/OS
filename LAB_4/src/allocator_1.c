// allocator1.c
#include "../inc/allocator_1.h"
#include <stdlib.h>
#include <string.h>

typedef struct Block {
    struct Block* next;
} Block;

typedef struct Allocator {
    Block* free_list;
    void* memory_start;
    size_t block_size;
    size_t memory_size;
} Allocator;

Allocator* allocator_create(void* const memory, const size_t size, const size_t block_size) {
    Allocator* allocator = (Allocator*)malloc(sizeof(Allocator));
    allocator->free_list = NULL;
    allocator->memory_start = memory;
    allocator->block_size = block_size;
    allocator->memory_size = size;

    char* current = (char*)memory;
    while (current + block_size <= (char*)memory + size) {
        Block* block = (Block*)current;
        block->next = allocator->free_list;
        allocator->free_list = block;
        current += block_size;
    }

    return allocator;
}

void allocator_destroy(Allocator* const allocator) {
    free(allocator);
}

void* allocator_alloc(Allocator* const allocator, const size_t size) {
    if (size > allocator->block_size || allocator->free_list == NULL) {
        return NULL;
    }
    Block* block = allocator->free_list;
    allocator->free_list = block->next;
    return (void*)block;
}

void allocator_free(Allocator* const allocator, void* const memory) {
    Block* block = (Block*)memory;
    block->next = allocator->free_list;
    allocator->free_list = block;
}