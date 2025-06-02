#include <gtest.h>

// Основная функция для запуска всех тестов Google Test

int main(int argc, char **argv) {
    // Инициализация Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Запуск всех тестов
    return RUN_ALL_TESTS();
}
