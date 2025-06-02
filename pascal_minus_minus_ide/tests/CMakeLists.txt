# Собираем все тестовые файлы
file(GLOB TEST_SOURCES "*.cpp")

# Создаем исполняемый файл для каждого теста
foreach(TEST_FILE ${TEST_SOURCES})
    # Получаем имя файла без расширения
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    
    # Создаем исполняемый файл для теста
    add_executable(${TEST_NAME} ${TEST_FILE})
    
    # Подключаем библиотеки pascal_core и gtest
    target_link_libraries(${TEST_NAME} pascal_core gtest gtest_main)
    
    # Добавляем тест в CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

# Создаем таргет для запуска всех тестов
add_custom_target(run_tests
    COMMAND ctest --output-on-failure
    DEPENDS ${TEST_SOURCES}
    COMMENT "Запуск всех тестов"
)

# Создаем новые дополнительные тесты

# Тест на обработку ошибок при выполнении программы
add_executable(test_error_handling test_error_handling.cpp)
target_link_libraries(test_error_handling pascal_core gtest gtest_main)
add_test(NAME test_error_handling COMMAND test_error_handling)

# Тест работы с циклом FOR
add_executable(test_for_loop test_for_loop.cpp)
target_link_libraries(test_for_loop pascal_core gtest gtest_main)
add_test(NAME test_for_loop COMMAND test_for_loop)

# Тест интеграции компонентов
add_executable(test_integration test_integration.cpp)
target_link_libraries(test_integration pascal_core gtest gtest_main)
add_test(NAME test_integration COMMAND test_integration)
