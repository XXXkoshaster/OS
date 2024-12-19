#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <semaphore.h>

#define SHM_SIZE 4096
#define SEM_NAME "/my_semaphore"
#define SHM_NAME "/my_shared_memory"

// Function declarations
void initialize_shared_memory(int *shm_fd, void **shm_ptr);
void cleanup_shared_memory(int shm_fd, void *shm_ptr);
void initialize_semaphore(sem_t **sem);
void cleanup_semaphore(sem_t *sem);

#endif // MAIN_H