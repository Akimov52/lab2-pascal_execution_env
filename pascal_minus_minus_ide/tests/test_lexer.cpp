#include <gtest/gtest.h>
#include "lexer.h"

// Проверка токенизации всех ключевых слов языка
TEST(LexerTest, TokenizeAllKeywords) {
    // Передаём строку, содержащую все ключевые слова
    Lexer lexer("program var begin end const if then else while do read readln write writeln");
    auto tokens = lexer.tokenize();

    // Проверяем, что каждый токен правильно распознан как ключевое слово
    EXPECT_EQ(tokens[0].type, TokenType::Program);
    EXPECT_EQ(tokens[1].type, TokenType::Var);
    EXPECT_EQ(tokens[2].type, TokenType::Begin);
    EXPECT_EQ(tokens[3].type, TokenType::End);
    EXPECT_EQ(tokens[4].type, TokenType::Const);
    EXPECT_EQ(tokens[5].type, TokenType::If);
    EXPECT_EQ(tokens[6].type, TokenType::Then);
    EXPECT_EQ(tokens[7].type, TokenType::Else);
    EXPECT_EQ(tokens[8].type, TokenType::While);
    EXPECT_EQ(tokens[9].type, TokenType::Do);
    EXPECT_EQ(tokens[10].type, TokenType::Read);
    EXPECT_EQ(tokens[11].type, TokenType::Readln);
    EXPECT_EQ(tokens[12].type, TokenType::Write);
    EXPECT_EQ(tokens[13].type, TokenType::Writeln);
}

// Проверка распознавания идентификаторов, чисел и строк
TEST(LexerTest, TokenizeIdentifiersNumbersStrings) {
    // Передаём смешанную строку с идентификаторами, числами и строковыми литералами
    Lexer lexer("x y z 123 456 'hello' 'world'");
    auto tokens = lexer.tokenize();

    // Проверка типов токенов
    EXPECT_EQ(tokens[0].type, TokenType::Identifier);  // x
    EXPECT_EQ(tokens[3].type, TokenType::Number);      // 123
    EXPECT_EQ(tokens[5].type, TokenType::String);      // 'hello'
    EXPECT_EQ(tokens[5].value, "hello");               // значение строки
    EXPECT_EQ(tokens[6].type, TokenType::String);      // 'world'
    EXPECT_EQ(tokens[6].value, "world");
}

// Проверка распознавания операторов и разделителей
TEST(LexerTest, TokenizeOperatorsAndDelimiters) {
    // Строка содержит все основные операторы и разделители
    Lexer lexer(":= + - * / = <> < <= > >= ; , . ( )");
    auto tokens = lexer.tokenize();

    // Проверка каждого токена на соответствие ожидаемому типу
    EXPECT_EQ(tokens[0].type, TokenType::Assign);
    EXPECT_EQ(tokens[1].type, TokenType::Plus);
    EXPECT_EQ(tokens[2].type, TokenType::Minus);
    EXPECT_EQ(tokens[3].type, TokenType::Mul);
    EXPECT_EQ(tokens[4].type, TokenType::Div);
    EXPECT_EQ(tokens[5].type, TokenType::Eq);
    EXPECT_EQ(tokens[6].type, TokenType::Neq);
    EXPECT_EQ(tokens[7].type, TokenType::Lt);
    EXPECT_EQ(tokens[8].type, TokenType::Le);
    EXPECT_EQ(tokens[9].type, TokenType::Gt);
    EXPECT_EQ(tokens[10].type, TokenType::Ge);
    EXPECT_EQ(tokens[11].type, TokenType::Semicolon);
    EXPECT_EQ(tokens[12].type, TokenType::Comma);
    EXPECT_EQ(tokens[13].type, TokenType::Dot);
    EXPECT_EQ(tokens[14].type, TokenType::LParen);
    EXPECT_EQ(tokens[15].type, TokenType::RParen);
}

// Проверка корректной токенизации при наличии различных пробелов и табуляции
TEST(LexerTest, TokenizeMixedWhitespace) {
    Lexer lexer("var   x\t:=  10 ;");
    auto tokens = lexer.tokenize();

    // Проверка, что лексер правильно игнорирует пробелы и табуляцию
    EXPECT_EQ(tokens[0].type, TokenType::Var);
    EXPECT_EQ(tokens[1].type, TokenType::Identifier);  // x
    EXPECT_EQ(tokens[2].type, TokenType::Assign);      // :=
    EXPECT_EQ(tokens[3].type, TokenType::Number);      // 10
    EXPECT_EQ(tokens[4].type, TokenType::Semicolon);   // ;
}

// Проверка, что лексер корректно игнорирует комментарии в фигурных скобках
TEST(LexerTest, TokenizeComments) {
    Lexer lexer("x := 1; { this is a comment } y := 2;");
    auto tokens = lexer.tokenize();

    // Комментарии не должны попасть в результат — только реальные токены
    EXPECT_EQ(tokens[0].type, TokenType::Identifier);  // x
    EXPECT_EQ(tokens[1].type, TokenType::Assign);      // :=
    EXPECT_EQ(tokens[2].type, TokenType::Number);      // 1
    EXPECT_EQ(tokens[3].type, TokenType::Semicolon);   // ;
    EXPECT_EQ(tokens[4].type, TokenType::Identifier);  // y
    EXPECT_EQ(tokens[5].type, TokenType::Assign);      // :=
    EXPECT_EQ(tokens[6].type, TokenType::Number);      // 2
    EXPECT_EQ(tokens[7].type, TokenType::Semicolon);   // ;
}

// Проверка, что лексер не падает при пустом вводе и возвращает пустой список токенов
TEST(LexerTest, TokenizeEmptyInput) {
    Lexer lexer("");
    auto tokens = lexer.tokenize();
    EXPECT_TRUE(tokens.empty()); // Ожидаем, что токенов нет
}

// Проверка, что лексер выбрасывает исключение при встрече невалидного символа
TEST(LexerTest, TokenizeInvalidSymbol) {
    Lexer lexer("@");
    EXPECT_THROW(lexer.tokenize(), std::runtime_error); // Ожидаем ошибку на невалидный символ
}
