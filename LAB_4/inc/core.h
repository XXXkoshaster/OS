#ifndef __CORE_H__
#define __CORE_H__

#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>

#define INT_SIZE 32
typedef struct thread_data
{
    int* array;
    int min;
    int max;
    int start;
    int end;
} thread_data;

void* find_min_max(void* arg);

#endif