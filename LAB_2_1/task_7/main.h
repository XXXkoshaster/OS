#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

typedef enum STATE {
    DONE = 0,
    DIR_PATH_NULL_PTR = 1,
    DIR_NOT_FOUND = 2,
    READING_ERROR = 3,
    STAT_ERROR = 4,
    UNAVALABLE_INPUT = 5
} STATE;

void handler(STATE state) {
    if (state == DONE)
        printf("Sucsessful finish\n");
    else if (state == DIR_PATH_NULL_PTR)
        printf("Direction path is null\n");
    else if (state == READING_ERROR)
        printf("Error of reading parh\n");
    else if (state == DIR_NOT_FOUND)
        printf("Direction not founded\n");
    else if (state == STAT_ERROR)
        printf("Stat error\n");
    else if (state == UNAVALABLE_INPUT)
        printf("Unavalable input\n");
    else
        printf("Unknown error\n");
}

#endif