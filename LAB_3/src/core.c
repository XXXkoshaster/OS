#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "pshm_ucase.h"

void create_shared_memory(const char *shmpath, struct shmbuf **shmp) {
    // Удаление объекта общей памяти, если он уже существует
    shm_unlink(shmpath);

    int fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd == -1) {
        errExit("shm_open");
    }

    if (ftruncate(fd, sizeof(struct shmbuf)) == -1) {
        errExit("ftruncate");
    }

    *shmp = mmap(NULL, sizeof(struct shmbuf), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (*shmp == MAP_FAILED) {
        errExit("mmap");
    }

    // Инициализация семафоров
    (*shmp)->sem1 = sem_open("/sem1", O_CREAT, 0644, 0);
    if ((*shmp)->sem1 == SEM_FAILED) {
        errExit("sem_open-sem1");
    }
    (*shmp)->sem2 = sem_open("/sem2", O_CREAT, 0644, 0);
    if ((*shmp)->sem2 == SEM_FAILED) {
        sem_unlink("/sem1");
        errExit("sem_open-sem2");
    }

    close(fd); // Закрытие файлового дескриптора после mmap
}

void cleanup_shared_memory(const char *shmpath, struct shmbuf *shmp) {
    if (shmp != NULL) {
        if (shmp->sem1 != NULL) {
            sem_close(shmp->sem1);
            sem_unlink("/sem1");
        }
        if (shmp->sem2 != NULL) {
            sem_close(shmp->sem2);
            sem_unlink("/sem2");
        }
        munmap(shmp, sizeof(struct shmbuf));
    }
    shm_unlink(shmpath);
}

void wait_for_child(struct shmbuf *shmp) {
    if (sem_wait(shmp->sem1) == -1) {
        errExit("sem_wait-sem1");
    }
}

void signal_parent(struct shmbuf *shmp) {
    if (sem_post(shmp->sem2) == -1) {
        errExit("sem_post-sem2");
    }
}