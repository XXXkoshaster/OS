#ifndef ALLOCATOR_2_H
#define ALLOCATOR_2_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define NUM_FREE_LISTS 32
#define MIN_BLOCK_SIZE 16

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


// Макрос для определения индекса в массиве freelist
#define NDX(size) \
    ((size) > 256 ? 4 : \
    (size) > 128 ? 3 : \
    (size) > 64 ? 2 : \
    (size) > 32 ? 1 : 0)

// Инициализация аллокатора на памяти memory размера size
struct Allocator* allocator_create(void* memory, size_t size);

// Деинициализация структуры аллокатора
void allocator_destroy(struct Allocator* allocator);

// Выделение памяти аллокатором памяти размера size
void* allocator_alloc(struct Allocator* allocator, size_t size);

// Возвращает выделенную память аллокатору
void allocator_free(struct Allocator* allocator, void* memory);

// Функция для получения индекса списка свободных блоков
int get_index(size_t size);

#endif // ALLOCATOR_2_H