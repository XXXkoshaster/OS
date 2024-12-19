#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#define SHM_SIZE 1024
#define SEM_NAME "/sem_example"

// Structure for shared memory
typedef struct {
    int data[SHM_SIZE];
    int count;
} SharedData;

// Function declarations
SharedData* init_shared_memory(key_t key);
void cleanup_shared_memory(SharedData* shared_data, int shmid);
void init_semaphore(sem_t** sem);
void cleanup_semaphore(sem_t* sem);

#endif // SHARED_MEMORY_H