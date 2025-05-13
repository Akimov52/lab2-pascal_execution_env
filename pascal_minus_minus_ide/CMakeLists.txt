cmake_minimum_required(VERSION 3.10)
project(pascal_minus_minus_ide VERSION 1.0.0)

# Устанавливаем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем исполняемый файл
add_executable(pascal_minus_minus_ide 
    source/main.cpp
    source/lexer.cpp
    source/parser.cpp
    source/interpreter.cpp
    source/symbol_table.cpp
)

# Включаем директории с заголовками
target_include_directories(pascal_minus_minus_ide PRIVATE include)

# Для Windows добавляем определение для консольного приложения
if(WIN32)
    target_link_libraries(pascal_minus_minus_ide PRIVATE kernel32.lib user32.lib)
    set_target_properties(pascal_minus_minus_ide PROPERTIES WIN32_EXECUTABLE FALSE)
endif()
