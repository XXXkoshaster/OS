#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>

#define BUFFER_SIZE 4096

typedef enum STATE {
    DONE = 0,
    ERROR_OPEN_FILE = 1,
    NULL_PTR = 2,
    UNAVALABLE_INPUT = 3,
    ERROR_CREATE_PID = 4
} STATE;

void handler(STATE state) {
    if (state == DONE)
        printf("Sucsessful finish\n");
    else if (state == ERROR_OPEN_FILE)
        printf("Direction path is null\n");
    else if (state == NULL_PTR)
        printf("Error of reading parh\n");
    else if (state == ERROR_CREATE_PID)
        printf("Pid creation error\n");
    else if (state == UNAVALABLE_INPUT)
        printf("Unavalable input\n");
    else
        printf("Unknown error\n");
}

#endif