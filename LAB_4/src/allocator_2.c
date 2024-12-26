#include "../inc/allocator_2.h"
#include <math.h>
#include <stdio.h> // Для отладочной информации
#include <stdbool.h>

struct Allocator* allocator_create(void* memory, size_t size) {
    if (size < (1 << NUM_FREE_LISTS)) return NULL;

    struct Allocator* allocator = (struct Allocator*)memory;
    allocator->base_memory = (char*)memory + sizeof(struct Allocator);
    allocator->memory_size = size - sizeof(struct Allocator);

    for (int i = 0; i <= NUM_FREE_LISTS; i++) {
        allocator->freelists[i] = NULL;
    }

    size_t initial_order = (size_t)log2(size);

    allocator->freelists[initial_order] = (struct FreeBlock*)allocator->base_memory;
    allocator->freelists[initial_order]->size = size;
    allocator->freelists[initial_order]->next = NULL;

    return allocator;
}

void allocator_destroy(struct Allocator* allocator) {
    // Сброс аллокатора в начальное состояние
    allocator->base_memory = NULL;
    allocator->memory_size = 0;
    // Очистка всех списков свободных блоков
    for (int i = 0; i <= NUM_FREE_LISTS; i++) {
        allocator->freelists[i] = NULL;
    }
}

void* allocator_alloc(struct Allocator* allocator, size_t size) {
    if (size == 0) return NULL;

    // Вычисление порядка для запрашиваемого размера
    size_t order = (size_t)ceil(log2(size + sizeof(struct FreeBlock)));
    if (order > NUM_FREE_LISTS) return NULL;

    // Поиск подходящего блока
    while (order <= NUM_FREE_LISTS) {
        if (allocator->freelists[order]) {
            struct FreeBlock* block = allocator->freelists[order];
            allocator->freelists[order] = block->next;

            // Разделение блока, если необходимо
            while (order > (size_t)ceil(log2(size + sizeof(struct FreeBlock)))) {
                order--;
                size_t block_size = 1 << order;
                struct FreeBlock* buddy = (struct FreeBlock*)((char*)block + block_size);
                buddy->size = block_size;
                buddy->next = allocator->freelists[order];
                allocator->freelists[order] = buddy;
            }

            block->size = 1 << order;
            return (char*)block + sizeof(struct FreeBlock);
        }
        order++;
    }

    return NULL;
}

void allocator_free(struct Allocator* allocator, void* memory) {
    if (!memory) return;

    struct FreeBlock* block = (struct FreeBlock*)((char*)memory - sizeof(struct FreeBlock));
    size_t order = (size_t)log2(block->size);

    // Добавляем блок в список свободных блоков
    block->next = allocator->freelists[order];
    allocator->freelists[order] = block;

    printf("Освобожден блок размером %zu на уровне %zu\n", block->size, order); // Отладочная информация

    // Пытаемся объединить блок с его близнецом
    while (order < NUM_FREE_LISTS) {
        size_t block_size = 1 << order;
        size_t buddy_address = ((size_t)block - (size_t)allocator->base_memory) ^ block_size;
        struct FreeBlock* buddy = (struct FreeBlock*)((char*)allocator->base_memory + buddy_address);

        // Проверяем, свободен ли близнец
        struct FreeBlock** list = &allocator->freelists[order];
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
                order++;
                block->next = allocator->freelists[order];
                allocator->freelists[order] = block;
                printf("Объединены блоки до размера %zu на уровне %zu\n", block->size, order); // Отладочная информация
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