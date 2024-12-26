#ifndef ALLOCATOR_1_H
#define ALLOCATOR_1_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

// Количество списков свободных блоков
#define NUM_FREE_LISTS 5

#define NDX(size) \
    (size) > 128 \
        ? (size) > 256 ? 4 : 3 \
        : (size) > 64 \
            ? 2 \
            : (size) > 32 ? 1 : 0

// Заголовок свободного блока
struct FreeBlock {
    size_t size;                // Размер блока
    struct FreeBlock* next;   // Указатель на следующий свободный блок
};


// Allocator structure
struct Allocator {
    void* base_memory;
    void* current_memory;
    size_t initial_size;
    size_t remaining_size;
    struct FreeBlock* freelist[NUM_FREE_LISTS + 1];
};

// Инициализация списков свободных блоков
void allocator_init(struct Allocator* allocator);

//Инициализация аллокатора на памяти memory размера size
struct Allocator* allocator_create(void* memory, size_t size);

//Деинициализация структуры аллокатора
void allocator_destroy(struct Allocator* allocator);

//Выделение памяти аллокатором памяти размера size
void* allocator_alloc(struct Allocator* allocator, size_t size);

//Возвращает выделенную память аллокатору
void allocator_free(struct Allocator* allocator, void* ptr);

#endif // ALLOCATOR1_H