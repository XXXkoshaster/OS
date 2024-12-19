#include "../inc/child.h"
#include "../inc/shared_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

int main() {
    int shm_fd;
    struct shared_data *data;

    shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("Error opening shared memory");
        exit(EXIT_FAILURE);
    }

    data = mmap(NULL, sizeof(struct shared_data), PROT_READ, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping shared memory");
        exit(EXIT_FAILURE);
    }

    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("Error opening semaphore");
        exit(EXIT_FAILURE);
    }

    while (1) {
        sem_wait(sem);
        
        if (data->ready) {
            int num = data->number;
            data->ready = 0; // Reset ready flag

            if (num < 0 || is_prime(num)) {
                exit(EXIT_SUCCESS);
            }

            if (!is_prime(num)) {
                data->result = num;
                data->result_ready = 1; // Indicate result is ready
            }
        }

        sem_post(sem);
        usleep(100); // Sleep briefly to avoid busy waiting
    }

    munmap(data, sizeof(struct shared_data));
    close(shm_fd);
    sem_close(sem);
    return 0;
}

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