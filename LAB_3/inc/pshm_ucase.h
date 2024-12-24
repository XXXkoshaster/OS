#ifndef PSHM_UCASE_H
#define PSHM_UCASE_H

#include <semaphore.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

#define BUF_SIZE 1024   /* Максимальный размер передаваемой строки */

/* Определение структуры, которая будет наложена на объект общей памяти */

struct shmbuf {
    sem_t  *sem1;            /* POSIX именованный семафор */
    sem_t  *sem2;            /* POSIX именованный семафор */
    size_t cnt;             /* Количество байт, используемых в 'buf' */
    char   buf[BUF_SIZE];   /* Передаваемые данные */
};

// Объявления функций
void create_shared_memory(const char *shmpath, struct shmbuf **shmp);
void cleanup_shared_memory(const char *shmpath, struct shmbuf *shmp);
void wait_for_child(struct shmbuf *shmp);
void signal_parent(struct shmbuf *shmp);
void wait_for_parent(struct shmbuf *shmp);
void signal_child(struct shmbuf *shmp);

#endif  // include guard