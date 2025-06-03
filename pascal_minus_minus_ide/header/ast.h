#ifndef AST_H
#define AST_H

/**
 * @file ast.h
 * @brief ����������� �������������� ������ (AST) ��� Pascal--
 * 
 * ���������� ��������� ������ ��� ������������� ��������� � ���� ������,
 * ��� �������� ������ � ������������� ����.
 */

#include <string>
#include <vector>
#include <memory>

// ��������������� ���������� ����� ��� ���������� ����������� ������������
struct Token;

// ���������� TokenType �� ������������ ���� PascalToken
namespace PascalToken { enum TokenType; }
using PascalToken::TokenType;

using namespace std;

/**
 * ������������ �������������� ����� ������ ��� �������������� ������� � �������� �����
 * ��� ������� ���� ������, �������������� � ����� Pascal--
 */
enum class DataType {
    Integer,    // ����� �����
    Real,       // ������������ �����
    Boolean,    // ���������� ���
    String      // ������
};

/**
 * �������� ���� ���������� ��� ���������
 * ��������� ��������� ��������� ������� ���� ������
 */
struct TypeInfo {
    DataType type;                         // ������� ��� ������

    explicit TypeInfo(DataType t) : type(t) {}
};

/**
 * ������������ ���� ��������� ����� ����� AST (������������ ��������������� ������)
 * ������ ��� ���� ������������ ������������ ����������� ����� Pascal--
 */
enum class ASTNodeType {
    Program,        // ������� ���������
    Block,          // ���� begin-end
    ConstSection,   // ������ ���������� ��������
    ConstDecl,      // ���������� ����� ���������
    VarSection,     // ������ ���������� ����������
    VarDecl,        // ���������� ����� ����������
    Assignment,     // ������������
    If,             // �������� ��������
    While,          // ���� while
    Write,          // �������� ������
    Writeln,        // �������� ������ � ��������� �� ����� ������
    Read,           // �������� �����
    Readln,         // �������� ����� � ��������� �� ����� ������
    Expression,     // ���������� ���������
    Number,         // ����� �����
    Real,           // ������������ �����
    String,         // ������
    Boolean,        // ���������� ��������
    Identifier,     // ������������� (��� ����������)
    BinOp,          // �������� �������� (+, -, *, /, � �.�.)
    UnOp,           // ������� �������� (��������, -a, not a)
    ForLoop         // for ... := ... to ... do ...
};

// ��������� ���� AST (������������ ��������������� ������)
class ASTNode {
public:
    ASTNodeType type;                              // ��� ����
    string value;                                  // �������� (��������, ��� ���������� ��� �������)
    vector<shared_ptr<ASTNode>> children;          // �������� ���� (��������, ���������, ���� �����)

    ASTNode(ASTNodeType t, const string& v = "") : type(t), value(v) {}
    ASTNode(ASTNodeType t, const string& v, const shared_ptr<ASTNode>& child) : type(t), value(v) { children.push_back(child); }
    virtual ~ASTNode() = default;
};

#endif // AST_H