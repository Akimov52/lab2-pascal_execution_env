cmake_minimum_required(VERSION 3.10)
project(pascal_minus_minus_ide)

# Включаем поддержку C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Устанавливаем директорию для бинарных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Добавляем подпроект pascal_minus_minus_ide
add_subdirectory(pascal_minus_minus_ide)
