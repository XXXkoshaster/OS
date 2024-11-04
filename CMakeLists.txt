cmake_minimum_required(VERSION 3.10)

# Название проекта
project(LAB_1)

# Установка стандарта C
set(CMAKE_C_STANDARD 99)

# Добавление исполняемого файла для родительского процесса
add_executable(parent ./LAB_1/parent.c)

# Добавление исполняемого файла для дочернего процесса
add_executable(child ./LAB_1/child.c)

# Включение директории include для поиска заголовочных файлов
