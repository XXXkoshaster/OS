#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

typedef void* (*allocator_create_t)(void*, size_t);
typedef void (*allocator_destroy_t)(void*);
typedef void* (*allocator_alloc_t)(void*, size_t);
typedef void (*allocator_free_t)(void*, void*);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path_to_library>\n", argv[0]);
        return EXIT_FAILURE;
    }

    void* handle = dlopen(argv[1], RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Failed to load library: %s\n", dlerror());
        return EXIT_FAILURE;
    }

    allocator_create_t allocator_create = (allocator_create_t)dlsym(handle, "allocator_create");
    allocator_destroy_t allocator_destroy = (allocator_destroy_t)dlsym(handle, "allocator_destroy");
    allocator_alloc_t allocator_alloc = (allocator_alloc_t)dlsym(handle, "allocator_alloc");
    allocator_free_t allocator_free = (allocator_free_t)dlsym(handle, "allocator_free");

    if (!allocator_create || !allocator_destroy || !allocator_alloc || !allocator_free) {
        fprintf(stderr, "Failed to load functions from library\n");
        dlclose(handle);
        return EXIT_FAILURE;
    }

    size_t memory_size = 1024 * 1024;
    void* memory = malloc(memory_size);

    void* allocator = allocator_create(memory, memory_size);
    void* block = allocator_alloc(allocator, 128);
    printf("Allocated block at: %p\n", block);

    allocator_free(allocator, block);
    allocator_destroy(allocator);

    dlclose(handle);
    free(memory);
    return EXIT_SUCCESS;
}