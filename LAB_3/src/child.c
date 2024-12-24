#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "../inc/pshm_ucase.h"

int is_prime(int num) {
    if (num <= 1)
        return 0;
    if (num <= 3)
        return 1;
    if (num % 2 == 0 || num % 3 == 0)
        return 0;
    for (int i = 5; i * i <= num; i += 6)
        if (num % i == 0 || num % (i + 2) == 0)
            return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <filename> <shmpath>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    const char *shmpath = argv[2];

    struct shmbuf *shmp;

    // Подключение к сегменту общей памяти
    int fd = shm_open(shmpath, O_RDWR, 0);
    if (fd == -1) 
        errExit("shm_open");
    

    shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) 
        errExit("mmap");

    // Перенаправление стандартного ввода на файл
    FILE *file = freopen(filename, "r", stdin);
    if (file == NULL) 
        errExit("freopen");

    // Проверка, является ли файл пустым
    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0) {
        printf("Дочерний процесс: файл пустой, завершение\n");
        shmp->cnt = 0; // Сигнализируем о завершении
        signal_parent(shmp);
        exit(EXIT_SUCCESS);
    }
    fseek(file, 0, SEEK_SET); // Возвращаемся в начало файла

    // Ожидание сигнала от родительского процесса
    printf("Дочерний процесс: ожидание семафора 1\n");
    wait_for_parent(shmp);

    printf("Дочерний процесс: чтение из стандартного ввода\n");
    char line[256];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        int num = atoi(line);

        printf("Дочерний процесс: прочитано число %d\n", num);

        if (num < 0 || is_prime(num)) {
            printf("Дочерний процесс: %d    простое, завершение\n", num);
            shmp->cnt = 0; // Сигнализируем о завершении
            signal_parent(shmp);

            exit(EXIT_SUCCESS);
        } else {
            printf("Дочерний процесс: %d составное, запись в общую память\n", num);
            shmp->cnt = snprintf(shmp->buf, sizeof(shmp->buf), "%d\n", num);
            signal_parent(shmp);

            // Ожидание, пока родительский процесс прочитает данные
            printf("Дочерний процесс: ожидание семафора 1 для продолжения\n");
            wait_for_parent(shmp);
        }
    }

    return 0;
}