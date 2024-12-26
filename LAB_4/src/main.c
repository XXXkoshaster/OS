#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <dlfcn.h> // Для динамической загрузки
#include <time.h>  // Для замера времени
#include <string.h> // Для использования strlen

// Указатели на функции аллокатора
static struct Allocator* (*allocator_create_func)(void*, size_t);
static void (*allocator_destroy_func)(struct Allocator*);
static void* (*allocator_alloc_func)(struct Allocator*, size_t);
static void (*allocator_free_func)(struct Allocator*, void*);

void write_message(const char* message) {
    write(STDOUT_FILENO, message, strlen(message));
}

void write_error(const char* message) {
    write(STDERR_FILENO, message, strlen(message));
}

void write_time(const char* prefix, double time) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s: %f секунд\n", prefix, time);
    write(STDOUT_FILENO, buffer, strlen(buffer));
}

int main(int argc, char **argv) {
    if (argc < 2) {
        write_error("Использование: <путь_к_библиотеке>\n");
        return EXIT_FAILURE;
    }

    // Загрузка динамической библиотеки
    void *library = dlopen(argv[1], RTLD_LOCAL | RTLD_NOW);
    if (!library) {
        write_error(dlerror());
        return EXIT_FAILURE;
    }

    // Загрузка функций аллокатора
    allocator_create_func = dlsym(library, "allocator_create");
    if (!allocator_create_func) {
        write_error(dlerror());
        return EXIT_FAILURE;
    }

    allocator_destroy_func = dlsym(library, "allocator_destroy");
    if (!allocator_destroy_func) {
        write_error(dlerror());
        return EXIT_FAILURE;
    }

    allocator_alloc_func = dlsym(library, "allocator_alloc");
    if (!allocator_alloc_func) {
        write_error(dlerror());
        return EXIT_FAILURE;
    }

    allocator_free_func = dlsym(library, "allocator_free");
    if (!allocator_free_func) {
        write_error(dlerror());
        return EXIT_FAILURE;
    }

    // Определение размера пула и выделение памяти
    size_t pool_size = 1024 * 1024; // Определение размера пула (например, 1MB)
    void* memory_pool = mmap(NULL, pool_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (memory_pool == MAP_FAILED) {
        write_error("Ошибка mmap\n");
        return EXIT_FAILURE;
    }

    // Создание аллокатора
    struct Allocator* allocator = allocator_create_func(memory_pool, pool_size);
    if (!allocator) {
        write_error("Ошибка создания аллокатора\n");
        munmap(memory_pool, pool_size);
        return EXIT_FAILURE;
    }

    // Определение границ пула памяти
    char* pool_start = (char*)memory_pool;
    char* pool_end = pool_start + pool_size;

    struct timespec start, end;
    double alloc_time, free_time;

    // Выделение 100 байт
    clock_gettime(CLOCK_MONOTONIC, &start);
    void* block1 = allocator_alloc_func(allocator, 100);
    clock_gettime(CLOCK_MONOTONIC, &end);
    alloc_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    write_time("Время выделения 100 байт", alloc_time);

    if (!block1) {
        write_error("Не удалось выделить 100 байт.\n");
    } else {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Выделено 100 байт по адресу %p\n", block1);
        write_message(buffer);
        char* block1_addr = (char*)block1;
        if (block1_addr < pool_start || block1_addr >= pool_end) {
            write_error("Block1 находится за пределами пула памяти.\n");
        }
    }

    // Выделение 200 байт
    clock_gettime(CLOCK_MONOTONIC, &start);
    void* block2 = allocator_alloc_func(allocator, 200);
    clock_gettime(CLOCK_MONOTONIC, &end);
    alloc_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    write_time("Время выделения 200 байт", alloc_time);

    if (!block2) {
        write_error("Не удалось выделить 200 байт.\n");
    } else {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Выделено 200 байт по адресу %p\n", block2);
        write_message(buffer);
        char* block2_addr = (char*)block2;
        if (block2_addr < pool_start || block2_addr >= pool_end) {
            write_error("Block2 находится за пределами пула памяти.\n");
        }
    }

    // Освобождение block1
    if (block1) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        allocator_free_func(allocator, block1);
        clock_gettime(CLOCK_MONOTONIC, &end);
        free_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        write_time("Время освобождения block1", free_time);
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Освобожден block1 по адресу %p\n", block1);
        write_message(buffer);
    }

    // Выделение 150 байт
    clock_gettime(CLOCK_MONOTONIC, &start);
    void* block3 = allocator_alloc_func(allocator, 150);
    clock_gettime(CLOCK_MONOTONIC, &end);
    alloc_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    write_time("Время выделения 150 байт", alloc_time);

    if (!block3) {
        write_error("Не удалось выделить 150 байт.\n");
    } else {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Выделено 150 байт по адресу %p\n", block3);
        write_message(buffer);
        char* block3_addr = (char*)block3;
        if (block3_addr < pool_start || block3_addr >= pool_end) {
            write_error("Block3 находится за пределами пула памяти.\n");
        }
        // Проверка, использовал ли block3 память block1
        if (block3 == block1) {
            write_message("Block3 использовал память block1.\n");
        } else {
            write_message("Block3 выделил новую память.\n");
        }
    }

    // Дополнительные тесты для проверки повторного использования памяти
    // Освобождение block2
    if (block2) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        allocator_free_func(allocator, block2);
        clock_gettime(CLOCK_MONOTONIC, &end);
        free_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        write_time("Время освобождения block2", free_time);
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Освобожден block2 по адресу %p\n", block2);
        write_message(buffer);
    }

    // Выделение 50 байт
    clock_gettime(CLOCK_MONOTONIC, &start);
    void* block4 = allocator_alloc_func(allocator, 50);
    clock_gettime(CLOCK_MONOTONIC, &end);
    alloc_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    write_time("Время выделения 50 байт", alloc_time);

    if (!block4) {
        write_error("Не удалось выделить 50 байт.\n");
    } else {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Выделено 50 байт по адресу %p\n", block4);
        write_message(buffer);
        char* block4_addr = (char*)block4;
        if (block4_addr < pool_start || block4_addr >= pool_end) {
            write_error("Block4 находится за пределами пула памяти.\n");
        }
        // Проверка, использовал ли block4 память block1
        if (block4 == block1) {
            write_message("Block4 использовал память block1.\n");
        } else {
            write_message("Block4 выделил новую память.\n");
        }
    }

    // Дополнительные тесты для проверки объединения блоков
    // Освобождение block3 и block4
    if (block3) {
        allocator_free_func(allocator, block3);
        printf("Освобожден block3 по адресу %p\n", block3);
    }
    if (block4) {
        allocator_free_func(allocator, block4);
        printf("Освобожден block4 по адресу %p\n", block4);
    }
    // Выделение 300 байт
    clock_gettime(CLOCK_MONOTONIC, &start);
    void* block5 = allocator_alloc_func(allocator, 300);
    clock_gettime(CLOCK_MONOTONIC, &end);
    alloc_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    write_time("Время выделения 300 байт", alloc_time);

    if (!block5) {
        write_error("Не удалось выделить 300 байт.\n");
    } else {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Выделено 300 байт по адресу %p\n", block5);
        write_message(buffer);
        char* block5_addr = (char*)block5;
        if (block5_addr < pool_start || block5_addr >= pool_end) {
            write_error("Block5 находится за пределами пула памяти.\n");
        }
    }

    // Уничтожение аллокатора
    allocator_destroy_func(allocator);
    munmap(memory_pool, pool_size);

    // Закрытие динамической библиотеки
    dlclose(library);

    write_message("Демонстрация работы аллокатора завершена.\n");

    return EXIT_SUCCESS;
}