// allocator1.c
#include "../inc/allocator_1.h"
#include <sys/mman.h> 

void allocator_init(struct Allocator* allocator) {
    for (int i = 0; i < NUM_FREE_LISTS; i++) {
        allocator->freelist[i] = NULL;
    }
}

// allocator_create: Инициализация аллокатора на памяти memory размера size
struct Allocator* allocator_create(void* memory, size_t size) {
    struct Allocator* allocator = (struct Allocator*)memory;
    allocator->base_memory = memory;
    allocator->current_memory = (void*)((char*)memory + sizeof(struct Allocator));
    allocator->initial_size = size - sizeof(struct Allocator);
    allocator->remaining_size = allocator->initial_size;
    allocator_init(allocator);
    return allocator;
}

// allocator_destroy: Деинициализация структуры аллокатора
void allocator_destroy(struct Allocator* allocator) {
    // Сброс аллокатора в начальное состояние
    allocator->current_memory = allocator->base_memory;
    allocator->remaining_size = allocator->initial_size;
    // Очистка всех списков свободных блоков
    for (int i = 0; i < NUM_FREE_LISTS; i++) {
        allocator->freelist[i] = NULL;
    }
    // Освобождение памяти пула
    munmap(allocator->base_memory, allocator->initial_size + sizeof(struct Allocator));
}

// allocator_alloc: Выделение памяти аллокатором памяти размера size
void* allocator_alloc(struct Allocator* allocator, size_t size) {
    if (size == 0) {
        return NULL;
    }

    // Корректировка размера для включения FreeBlock и выравнивание, если необходимо
    size_t alloc_size = sizeof(struct FreeBlock) + size;

    // Поиск подходящего списка свободных блоков
    int index = NDX(size);
    struct FreeBlock* block = allocator->freelist[index];

    if (block != NULL) {
        // Использование первого блока в списке свободных блоков
        allocator->freelist[index] = block->next;
        // Установка размера в заголовке блока
        block->size = size;
        // Возврат памяти после заголовка
        return (void*)(block + 1);
    } else {
        // Выделение нового блока из пула памяти
        void* ptr = allocator->current_memory;
        if (ptr == NULL) {
            return NULL;
        }
        // Проверка, достаточно ли осталось памяти
        if (allocator->remaining_size < alloc_size) {
            return NULL;
        }
        // Выделение блока
        block = (struct FreeBlock*)ptr;
        block->size = size;
        // Обновление указателя на текущую память и оставшегося размера
        allocator->current_memory = (void*)((char*)allocator->current_memory + alloc_size);
        allocator->remaining_size -= alloc_size;
        // Возврат памяти после заголовка
        return (void*)(block + 1);
    }
}

// allocator_free: Возвращает выделенную память аллокатору
void allocator_free(struct Allocator* allocator, void* ptr) {
    if (ptr == NULL) {
        return;
    }

    // Получение заголовка блока
    struct FreeBlock* block = (struct FreeBlock*)ptr - 1;
    size_t size = block->size;

    // Определение, какой список свободных блоков использовать
    int index = NDX(size);

    // Вставка блока в список свободных блоков
    block->next = allocator->freelist[index];
    allocator->freelist[index] = block;
}