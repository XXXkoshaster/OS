cmake_minimum_required(VERSION 3.10)
project(AllocatorProject)

set(CMAKE_C_STANDARD 99)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/inc)

# Add the shared libraries
add_library(allocator_1 SHARED src/allocator_1.c)
add_library(allocator_2 SHARED src/allocator_2.c)

# Add the executable
add_executable(core src/main.c)

# Set policy CMP0079 to NEW
cmake_policy(SET CMP0079 NEW)

# Link the shared library to the executable
target_link_libraries(core PRIVATE allocator_1 dl)