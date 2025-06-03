#ifndef PARSER_H
#define PARSER_H

/**
 * @file parser.h
 * @brief �������������� ���������� (������) ��� ����� Pascal--
 * 
 * �������� �� ���������� ������������ ��������������� ������ (AST)
 * �� ������������������ �������, ���������� �� �������.
 */

#include "ast.h"
#include "lexer.h"
#include "interfaces.h"
#include "error_reporter.h"

/**
 * ����� ��������������� ����������� (�������)
 * ��������� ���������� ����������� ������ � �������������� ��� ���������� AST.
 * ������������� ������ ��� ������� ���� ����������� ����� Pascal--
 */
class Parser : public IParser {
public:
    /**
     * ����������� �� ���������
     * ������� ������ ��� ��������� ������� � ����������� ������
     */
    Parser();
    
    /**
     * ����������� � ������������ ������
     * @param errorReporter ��������� �� ���������� ������
     */
    explicit Parser(std::shared_ptr<IErrorReporter> errorReporter);

    /**
     * ����������� ��������� ������ �������, ���������� �� �������
     * @param tokens ������ ������� ��� �������
     * @param errorReporter ������������ ���������� ������
     */
    explicit Parser(const vector<Token>& tokens, std::shared_ptr<IErrorReporter> errorReporter = nullptr);

    /**
     * ���������� ������ ���������� IParser ��� ��������������� �������
     * @return ��������� �� �������� ���� ������������ ������������ ��������������� ������
     */
    shared_ptr<ASTNode> parse() override;
    
    /**
     * ���������� ��� ����������
     * @return ������ � ������ ���������� ("Parser")
     */
    std::string getComponentName() const override { return "Parser"; }

private:
    std::vector<Token> tokens;   // ������ ������� ��� �������
    size_t pos;                  // ������� ������� � ������ �������
    std::shared_ptr<IErrorReporter> errorReporter; // ���������� ������

    // �������� ������� �����
    const Token& current() const;
    // ���� ������� ����� ��������� � ��������� ����� � ������� � ����������
    bool match(TokenType type);
    // ���������, ��� ������� ����� ������� ����, ����� ��������� ���������� � ����������
    void expect(TokenType type, const string& errorMsg);

    // ������ ������� ��������� ����������� �����
    shared_ptr<ASTNode> parseProgram();          // program ... end.
    shared_ptr<ASTNode> parseBlock();            // begin ... end
    shared_ptr<ASTNode> parseStatement();        // �������� (������������, if, while � �.�.)
    shared_ptr<ASTNode> parseAssignment();       // ������������
    shared_ptr<ASTNode> parseIf();               // if ... then ... else
    shared_ptr<ASTNode> parseWhile();            // while ... do ...
    shared_ptr<ASTNode> parseWrite();            // Write(...)
    shared_ptr<ASTNode> parseRead();             // Read(...)
    shared_ptr<ASTNode> parseReadln();           // Readln(...)
    shared_ptr<ASTNode> parseWriteln();          // Write(...)
    shared_ptr<ASTNode> parseExpression();       // ����� ���������
    shared_ptr<ASTNode> parseSimpleExpression(); // ������� ��������� (��� ������)
    shared_ptr<ASTNode> parseTerm();             // ����� (���������, ������� � �.�.)
    shared_ptr<ASTNode> parseFactor();           // ������� (�����, ��������������, ������)
    shared_ptr<ASTNode> parseVarSection();       // var ... ;
    shared_ptr<ASTNode> parseConstSection();     // const ... ;
    shared_ptr<ASTNode> parseFor();              // for ... to ... do ...

};

#endif // PARSER_H