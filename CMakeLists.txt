cmake_minimum_required(VERSION 3.10)

# Название проекта
project(OS)

set(CMAKE_C_STANDARD 99)

# Компиляционные флаги
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -std=c99 -g -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment")

# Линковочные флаги
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")

set(CMAKE_C_FLAGS_LABS "${CMAKE_C_FLAGS} -lm"
    CACHE STRING "Compiler flags in labs build"
    FORCE)

message("General compiler flags: " ${CMAKE_C_FLAGS})
message("Debug build compiler flags: " ${CMAKE_C_FLAGS})
message("Labs build compiler flags: " ${CMAKE_C_FLAGS})

add_subdirectory(LAB_1)
add_subdirectory(LAB_2)