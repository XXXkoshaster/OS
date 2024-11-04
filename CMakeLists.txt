cmake_minimum_required(VERSION 3.10)

# Название проекта
project(LAB_1)

set(CMAKE_C_STANDARD 99)

add_executable(parent ./LAB_1/src/main.c ../LAB_1/src/core.c)

add_executable(child ./LAB_1/src/child.c)

target_include_directories(parent PRIVATE ./inc)

target_include_directories(parent PRIVATE ./inc)