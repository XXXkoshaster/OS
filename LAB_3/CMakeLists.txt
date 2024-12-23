cmake_minimum_required(VERSION 3.10)

# Название проекта
project(LAB_3)

set(CMAKE_C_STANDARD 99)

add_executable(shm_parent ./src/main.c ./src/core.c)
add_executable(shm_child ./src/child.c)

target_include_directories(shm_parent PRIVATE ./inc)
target_include_directories(shm_child PRIVATE ./inc)

target_compile_options(shm_parent PRIVATE -g)
target_compile_options(shm_child PRIVATE -g)