#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../inc/pshm_ucase.h"

int main() {
    char shmpath[256];
    snprintf(shmpath, sizeof(shmpath), "/my_shared_memory");

    struct shmbuf *shmp;
    create_shared_memory(shmpath, &shmp);

    char filename[256];
    printf("Введите имя файла: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }
    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }

    pid_t child = fork();
    if (child == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child == 0) {
        // Дочерний процесс
        printf("Дочерний процесс: выполнение дочернего процесса\n");
        execlp("./shm_child", "./shm_child", filename, shmpath, NULL); // Используем относительный путь
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // Родительский процесс
        // Сигнализируем дочернему процессу о готовности
        if (sem_post(shmp->sem1) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }

        while (1) {
            printf("Родительский процесс: ожидание семафора 2\n");
            if (sem_wait(shmp->sem2) == -1) {
                perror("sem_wait");
                exit(EXIT_FAILURE);
            }

            if (shmp->cnt == 0) {
                break; // Выход из цикла, если дочерний процесс сигнализирует о завершении
            }

            printf("Родительский процесс: чтение из общей памяти\n");
            write(STDOUT_FILENO, shmp->buf, shmp->cnt);
            write(STDOUT_FILENO, "\n", 1);

            // Сигнализируем дочернему процессу продолжить
            printf("Родительский процесс: сигнализируем дочернему процессу продолжить\n");
            if (sem_post(shmp->sem1) == -1) {
                perror("sem_post");
                exit(EXIT_FAILURE);
            }
        }

        wait(NULL); // Ожидание завершения дочернего процесса
        cleanup_shared_memory(shmpath, shmp);
    }

    return 0;
}