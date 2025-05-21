#ifndef AST_H
#define AST_H

#include "lexer.h" // ��� TokenType

// ������������ �������������� ����� ������ ��� �������������� ������� � �������� �����
enum class DataType {
    Integer,    // ����� �����
    Real,       // ������������ �����
    Boolean,    // ���������� ���
    String,     // ������
    Array,      // ������
    Procedure,  // ���������
    Function,   // �������
    Void        // ������ ��� (��������, ��� ��������)
};

// �������� ���� ���������� ��� ���������, ������� ��������� ���� ��� ��������
struct TypeInfo {
    DataType type;                         // ������� ��� ������
    shared_ptr<TypeInfo> elementType;      // ��� ��������� (��� ��������)
    int arraySize = 0;                     // ������ ������� (���� ���������)

    explicit TypeInfo(DataType t) : type(t), elementType(nullptr), arraySize(0) {}
};

// ������������ ���� ��������� ����� ����� AST (������������ ��������������� ������)
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
    Identifier,     // ������������� (��� ����������, ������� � �.�.)
    ProcCall,       // ����� ���������
    BinOp,          // �������� �������� (+, -, *, /, � �.�.)
    UnOp,           // ������� �������� (��������, -a, not a)
    ArrayDecl,      // ���������� �������
    ArrayAccess,    // ��������� � �������� �������
    ProcedureDecl,  // ���������� ���������
    FunctionDecl,   // ���������� �������
    Call,           // ����� �������/���������
    Return          // �������� ��������
};

// ��������� ���� AST (������������ ��������������� ������)
struct ASTNode {
    ASTNodeType type;                              // ��� ����
    string value;                                  // �������� (��������, ��� ���������� ��� �������)
    vector<shared_ptr<ASTNode>> children;          // �������� ���� (��������, ���������, ���� �����)

    ASTNode(ASTNodeType t, const string& v = "") : type(t), value(v) {}
    virtual ~ASTNode() = default;
};

#endif // AST_H