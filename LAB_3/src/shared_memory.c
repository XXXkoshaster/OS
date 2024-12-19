#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include "shared_memory.h"

#define SHM_SIZE 1024

int create_shared_memory(key_t key) {
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

void* attach_shared_memory(int shmid) {
    void* shm_addr = shmat(shmid, NULL, 0);
    if (shm_addr == (void*) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    return shm_addr;
}

void detach_shared_memory(void* shm_addr) {
    if (shmdt(shm_addr) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

void delete_shared_memory(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
}

int create_semaphore(key_t key) {
    int semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid < 0) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    return semid;
}

void initialize_semaphore(int semid) {
    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }
}

void wait_semaphore(int semid) {
    struct sembuf sb = {0, -1, 0};
    if (semop(semid, &sb, 1) == -1) {
        perror("semop wait");
        exit(EXIT_FAILURE);
    }
}

void signal_semaphore(int semid) {
    struct sembuf sb = {0, 1, 0};
    if (semop(semid, &sb, 1) == -1) {
        perror("semop signal");
        exit(EXIT_FAILURE);
    }
}