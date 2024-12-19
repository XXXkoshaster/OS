cmake_minimum_required(VERSION 3.10)

# Название проекта
project(LAB_3)

set(CMAKE_C_STANDARD 99)

add_executable(parent ./src/main.c ./src/core.c)
add_executable(child ./src/child.c)

target_include_directories(parent PRIVATE ./inc)
target_include_directories(parent PRIVATE ./inc)

target_compile_options(parent PRIVATE -g)
target_compile_options(child PRIVATE -g)