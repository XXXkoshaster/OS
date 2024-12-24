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
    if (fgets(filename, sizeof(filename), stdin) == NULL) 
        errExit("fgets");
    
    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }

    pid_t child = fork();
    if (child == -1) 
        errExit("fork");

    if (child == 0) {
        // Дочерний процесс
        printf("Дочерний процесс: выполнение дочернего процесса\n");
        execlp("./shm_child", "./shm_child", filename, shmpath, NULL); 
        errExit("execlp");
    } else {
        // Родительский процесс
        // Сигнализируем дочернему процессу о готовности
        signal_child(shmp);

        while (1) {
            printf("Родительский процесс: ожидание семафора 2\n");
            wait_for_child(shmp);

            if (shmp->cnt == 0) {
                break; // Выход из цикла, если дочерний процесс сигнализирует о завершении
            }

            printf("Родительский процесс: чтение из общей памяти\n");
            write(STDOUT_FILENO, shmp->buf, shmp->cnt);
            write(STDOUT_FILENO, "\n", 1);

            // Сигнализируем дочернему процессу продолжить
            printf("Родительский процесс: сигнализируем дочернему процессу продолжить\n");
            signal_child(shmp);
        }

        wait(NULL); // Ожидание завершения дочернего процесса
        cleanup_shared_memory(shmpath, shmp);
    }

    return 0;
}