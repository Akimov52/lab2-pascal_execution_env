cmake_minimum_required(VERSION 3.10)
project(pascal_minus_minus_ide)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# Добавляем библиотеку
add_library(pascal_minus_minus_ide_lib
    pascal_minus_minus_ide_lib/source/lexer.cpp
    pascal_minus_minus_ide_lib/source/parser.cpp
    pascal_minus_minus_ide_lib/source/interpreter.cpp
    pascal_minus_minus_ide_lib/source/postfix.cpp
    pascal_minus_minus_ide_lib/source/error_reporter.cpp
)

target_include_directories(pascal_minus_minus_ide_lib PUBLIC
    pascal_minus_minus_ide_lib/header
)

# Добавляем тесты
add_executable(pascal_minus_minus_ide_tests
    pascal_minus_minus_ide_tests/source/test_main.cpp
    pascal_minus_minus_ide_tests/source/test_lexer.cpp
    pascal_minus_minus_ide_tests/source/test_parser.cpp
    pascal_minus_minus_ide_tests/source/test_interpreter.cpp
    pascal_minus_minus_ide_tests/source/test_postfix.cpp
)

target_include_directories(pascal_minus_minus_ide_tests PRIVATE
    pascal_minus_minus_ide_lib/header
)

target_link_libraries(pascal_minus_minus_ide_tests
    pascal_minus_minus_ide_lib
    GTest::gtest_main
)

# Включаем тестирование
include(GoogleTest)
gtest_discover_tests(pascal_minus_minus_ide_tests) 