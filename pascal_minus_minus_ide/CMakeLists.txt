# Добавляем путь к заголовочным файлам
include_directories(include)

# Собираем все исходные файлы в директории source
file(GLOB SOURCES "source/*.cpp")

# Исключаем main.cpp из библиотеки
list(FILTER SOURCES EXCLUDE REGEX ".*main\.cpp$")

# Создаем библиотеку pascal_core со всеми компонентами интерпретатора
add_library(pascal_core ${SOURCES})

# Создаем исполняемый файл иде
add_executable(pascal_minus_minus_ide source/main.cpp)

# Подключаем библиотеку к исполняемому файлу
target_link_libraries(pascal_minus_minus_ide pascal_core)

# Настройка Google Test
enable_testing()

# Добавляем кастомный таргет для библиотеки Google Test
add_library(gtest STATIC gtest/gtest-all.cc)
add_library(gtest_main STATIC gtest/gtest-all.cc)
target_compile_definitions(gtest PUBLIC GTEST_LANG_CXX11)
target_compile_definitions(gtest_main PUBLIC GTEST_LANG_CXX11)
target_compile_definitions(gtest_main PUBLIC GTEST_HAS_MAIN)

# Создаем директорию для тестов
add_subdirectory(tests)
