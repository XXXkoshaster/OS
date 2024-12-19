#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include "main.h"
#include "shared_memory.h"
#include "child.h"

#define SHM_NAME "/my_shared_memory"
#define SEM_WRITE_NAME "/my_semaphore_write"
#define SEM_READ_NAME "/my_semaphore_read"

int main() {
    int shm_fd;
    SharedData *data;

    // Create shared memory
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedData));
    data = mmap(0, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Initialize semaphores
    sem_t *sem_write = sem_open(SEM_WRITE_NAME, O_CREAT, 0666, 1);
    sem_t *sem_read = sem_open(SEM_READ_NAME, O_CREAT, 0666, 0);

    // Create child process
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        handle_child_process(data, sem_write, sem_read);
    } else {
        // Parent process
        handle_parent_process(data, sem_write, sem_read, pid);
    }

    // Cleanup
    munmap(data, sizeof(SharedData));
    shm_unlink(SHM_NAME);
    sem_close(sem_write);
    sem_close(sem_read);
    sem_unlink(SEM_WRITE_NAME);
    sem_unlink(SEM_READ_NAME);

    return 0;
}