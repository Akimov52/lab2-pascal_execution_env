#include <gtest.h>
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

// Проверка токенизации ключевых слов цикла for
TEST(LexerTest, TokenizeForLoop) {
    Lexer lexer("for i := 1 to 10 do begin end;");
    auto tokens = lexer.tokenize();
    
    // Проверяем ключевые слова for, to, do
    EXPECT_EQ(tokens[0].type, TokenType::For);
    EXPECT_EQ(tokens[3].type, TokenType::To);  // После "i :="
    EXPECT_EQ(tokens[5].type, TokenType::Do);  // После "10"
    
    // Проверяем идентификатор и числа
    EXPECT_EQ(tokens[1].type, TokenType::Identifier);
    EXPECT_EQ(tokens[1].value, "i");
    EXPECT_EQ(tokens[4].type, TokenType::Number);
    EXPECT_EQ(tokens[4].value, "10");
}

// Проверка токенизации ключевого слова downto
TEST(LexerTest, TokenizeForDownto) {
    Lexer lexer("for i := 10 downto 1 do x := x - 1;");
    auto tokens = lexer.tokenize();
    
    // Проверяем ключевое слово downto
    EXPECT_EQ(tokens[0].type, TokenType::For);
    EXPECT_EQ(tokens[3].type, TokenType::Downto);  // После "10"
}



// Проверка токенизации логических операторов
TEST(LexerTest, TokenizeLogicalOperators) {
    Lexer lexer("x and y or not z");
    auto tokens = lexer.tokenize();
    
    // Проверяем логические операторы
    EXPECT_EQ(tokens[0].type, TokenType::Identifier); // x
    EXPECT_EQ(tokens[1].type, TokenType::And);
    EXPECT_EQ(tokens[2].type, TokenType::Identifier); // y
    EXPECT_EQ(tokens[3].type, TokenType::Or);
    EXPECT_EQ(tokens[4].type, TokenType::Not);
    EXPECT_EQ(tokens[5].type, TokenType::Identifier); // z
}

// Проверка токенизации булевых констант
TEST(LexerTest, TokenizeBooleanConstants) {
    Lexer lexer("true false");
    auto tokens = lexer.tokenize();
    
    // Проверяем булевы константы
    EXPECT_EQ(tokens[0].type, TokenType::True);
    EXPECT_EQ(tokens[1].type, TokenType::False);
}

// Проверка токенизации нескольких вложенных комментариев
TEST(LexerTest, TokenizeNestedComments) {
    Lexer lexer("x := 1; { outer comment { inner comment } still in comment } y := 2;");
    auto tokens = lexer.tokenize();
    
    // Проверяем, что комментарии корректно пропущены
    EXPECT_EQ(tokens[0].type, TokenType::Identifier);  // x
    EXPECT_EQ(tokens[1].type, TokenType::Assign);      // :=
    EXPECT_EQ(tokens[2].type, TokenType::Number);      // 1
    EXPECT_EQ(tokens[3].type, TokenType::Semicolon);   // ;
    EXPECT_EQ(tokens[4].type, TokenType::Identifier);  // y
    EXPECT_EQ(tokens[5].type, TokenType::Assign);      // :=
    EXPECT_EQ(tokens[6].type, TokenType::Number);      // 2
    EXPECT_EQ(tokens[7].type, TokenType::Semicolon);   // ;
}

// Проверка токенизации вещественных чисел
TEST(LexerTest, TokenizeRealNumbers) {
    Lexer lexer("3.14 2.0 0.5");
    auto tokens = lexer.tokenize();
    
    // Проверяем вещественные числа
    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, "3.14");
    EXPECT_EQ(tokens[1].type, TokenType::Number);
    EXPECT_EQ(tokens[1].value, "2.0");
    EXPECT_EQ(tokens[2].type, TokenType::Number);
    EXPECT_EQ(tokens[2].value, "0.5");
}
