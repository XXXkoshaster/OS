cmake_minimum_required(VERSION 3.10)

# Название проекта
project(LAB_2)

set(CMAKE_C_STANDARD 99)

add_executable(main ./src/main.c)

target_include_directories(main PRIVATE ./inc)

target_compile_options(main PRIVATE -g)