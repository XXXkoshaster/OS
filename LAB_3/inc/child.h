#ifndef CHILD_H
#define CHILD_H

#include <stdint.h>

void process_data_from_shared_memory(int shm_id, int sem_id);
int is_prime(int num);

#endif // CHILD_H