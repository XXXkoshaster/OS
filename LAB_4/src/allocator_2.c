#include "../inc/allocator_2.h"
#include <math.h>
#include <stdio.h> // Для отладочной информации
#include <stdbool.h>
#include <sys/mman.h> // Для функции munmap

void allocator_init(struct Allocator* allocator) {
    for (int i = 0; i < NUM_FREE_LISTS; i++) {
        allocator->freelist[i] = NULL;
    }
}

// Функция для округления размера до ближайшей степени двойки
size_t round_up_to_power_of_two(size_t size) {
    if (size < MIN_BLOCK_SIZE) {
        return MIN_BLOCK_SIZE;
    }
    return (size_t)pow(2, ceil(log2(size)));
}

struct Allocator* allocator_create(void* memory, size_t size) {
    // Округляем размер памяти до ближайшей степени двойки
    size_t rounded_size = round_up_to_power_of_two(size);

    // Проверяем, что выделенная память достаточно велика для структуры Allocator
    if (rounded_size < sizeof(struct Allocator)) {
        return NULL;
    }

    struct Allocator* allocator = (struct Allocator*)memory;
    allocator->base_memory = memory;
    allocator->current_memory = (void*)((char*)memory + sizeof(struct Allocator));
    allocator->initial_size = rounded_size - sizeof(struct Allocator);
    allocator->remaining_size = allocator->initial_size;
    allocator_init(allocator);

    // Инициализация первого свободного блока
    if (allocator->remaining_size >= MIN_BLOCK_SIZE) {
        struct FreeBlock* block = (struct FreeBlock*)allocator->current_memory;
        block->size = allocator->remaining_size;
        block->next = NULL;
        int index = get_index(block->size);
        allocator->freelist[index] = block;
    }

    return allocator;
}

void allocator_destroy(struct Allocator* allocator) {
    allocator->current_memory = allocator->base_memory;
    allocator->remaining_size = allocator->initial_size;
    for (int i = 0; i < NUM_FREE_LISTS; i++) {
        allocator->freelist[i] = NULL;
    }
    munmap(allocator->base_memory, allocator->initial_size + sizeof(struct Allocator));
}

int get_index(size_t size) {
    int index = 0;
    while ((1 << index) < size) {
        index++;
    }
    return index;
}

void* allocator_alloc(struct Allocator* allocator, size_t size) {
    if (size == 0) {
        return NULL;
    }

    size_t alloc_size = sizeof(struct FreeBlock) + size;
    int index = get_index(alloc_size);

    if (index >= NUM_FREE_LISTS) {
        return NULL;
    }

    struct FreeBlock* block = allocator->freelist[index];

    if (block != NULL) {
        allocator->freelist[index] = block->next;
        block->size = size;
        return (void*)(block + 1);
    } else {
        if (index < NUM_FREE_LISTS - 1) {
            void* ptr = allocator_alloc(allocator, (1 << (index + 1)) - sizeof(struct FreeBlock));
            if (ptr == NULL) {
                return NULL;
            }
            block = (struct FreeBlock*)ptr - 1;
            block->size = size;
            struct FreeBlock* buddy = (struct FreeBlock*)((char*)ptr + (1 << index));
            buddy->size = (1 << index) - sizeof(struct FreeBlock);
            buddy->next = allocator->freelist[index];
            allocator->freelist[index] = buddy;
            return ptr;
        } else {
            if (allocator->remaining_size < alloc_size) {
                return NULL;
            }
            block = (struct FreeBlock*)allocator->current_memory;
            block->size = size;
            allocator->current_memory = (void*)((char*)allocator->current_memory + alloc_size);
            allocator->remaining_size -= alloc_size;
            return (void*)(block + 1);
        }
    }
}

void allocator_free(struct Allocator* allocator, void* ptr) {
    if (ptr == NULL) {
        return;
    }

    struct FreeBlock* block = (struct FreeBlock*)ptr - 1;
    size_t size = block->size;
    int index = get_index(size + sizeof(struct FreeBlock));

    // Добавляем блок в список свободных блоков
    block->next = allocator->freelist[index];
    allocator->freelist[index] = block;

    printf("Освобожден блок размером %zu на уровне %d\n", block->size, index); // Отладочная информация

    // Пытаемся объединить блок с его близнецом
    while (index < NUM_FREE_LISTS) {
        size_t block_size = 1 << index;
        size_t buddy_address = ((size_t)block - (size_t)allocator->base_memory) ^ block_size;
        struct FreeBlock* buddy = (struct FreeBlock*)((char*)allocator->base_memory + buddy_address);

        // Проверяем, свободен ли близнец
        struct FreeBlock** list = &allocator->freelist[index];
        struct FreeBlock* prev = NULL;
        struct FreeBlock* curr = *list;

        bool buddy_found = false;
        while (curr) {
            if (curr == buddy) {
                // Удаляем близнеца из списка свободных блоков
                if (prev) {
                    prev->next = curr->next;
                } else {
                    *list = curr->next;
                }

                // Объединяем блоки
                if ((size_t)buddy < (size_t)block) {
                    block = buddy;
                }

                block->size = block_size * 2;
                index++;
                block->next = allocator->freelist[index];
                allocator->freelist[index] = block;
                printf("Объединены блоки до размера %zu на уровне %d\n", block->size, index); // Отладочная информация
                buddy_found = true;
                break;
            }
            prev = curr;
            curr = curr->next;
        }

        if (!buddy_found) {
            break; // Близнец не найден, прекращаем объединение
        }
    }
}