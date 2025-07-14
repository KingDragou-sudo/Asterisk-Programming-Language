#pragma once
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include "../lexer.hpp"

struct Statement;
struct Expression;

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

struct Expression : public ASTNode {
    virtual ~Expression() = default;
};


struct Statement : public ASTNode {
    virtual ~Statement() = default;
};

struct IntLiteral : public Expression {
    int value;

    IntLiteral(int val) : value(val) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "IntLiteral: " << value << std::endl;
    }
};

struct FloatLiteral : public Expression {
    float value;

    FloatLiteral(float val) : value(val) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "FloatLiteral: " << value << std::endl;
    }
};

struct StringLiteral : public Expression {
    std::string value;

    StringLiteral(const std::string& val) : value(val) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "StringLiteral: \"" << value << "\"" << std::endl;
    }
};

struct BooleanLiteral : public Expression {
    bool value;

    BooleanLiteral(bool val) : value(val) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "BooleanLiteral: " << (value ? "true" : "false") << std::endl;
    }
};

struct Identifier : public Expression {
    std::string name;

    Identifier(const std::string& n) : name(n) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Identifier: " << name << std::endl;
    }
};

struct BinaryExpression : public Expression {
    std::unique_ptr<Expression> left;
    TokenType operator_type;
    std::unique_ptr<Expression> right;

    BinaryExpression(std::unique_ptr<Expression> l, TokenType op, std::unique_ptr<Expression> r)
        : left(std::move(l)), operator_type(op), right(std::move(r)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "BinaryExpression:" << std::endl;
        std::cout << std::string(indent + 2, ' ') << "Operator: " << operator_type << std::endl;
        std::cout << std::string(indent + 2, ' ') << "Left:" << std::endl;
        left->print(indent + 4);
        std::cout << std::string(indent + 2, ' ') << "Right:" << std::endl;
        right->print(indent + 4);
    }
};

struct UnaryExpression : public Expression {
    TokenType operator_type;
    std::unique_ptr<Expression> operand;

    UnaryExpression(TokenType op, std::unique_ptr<Expression> expr)
        : operator_type(op), operand(std::move(expr)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "UnaryExpression:" << std::endl;
        std::cout << std::string(indent + 2, ' ') << "Operator: " << operator_type << std::endl;
        std::cout << std::string(indent + 2, ' ') << "Operand:" << std::endl;
        operand->print(indent + 4);
    }
};

struct ParenthesizedExpression : public Expression {
    std::unique_ptr<Expression> expression;

    ParenthesizedExpression(std::unique_ptr<Expression> expr) : expression(std::move(expr)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "ParenthesizedExpression:" << std::endl;
        expression->print(indent + 2);
    }
};

struct FunctionCall : public Expression {
    std::string function_name;
    std::vector<std::unique_ptr<Expression>> arguments;

    FunctionCall(const std::string& name, std::vector<std::unique_ptr<Expression>> args)
        : function_name(name), arguments(std::move(args)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "FunctionCall: " << function_name << std::endl;
        std::cout << std::string(indent + 2, ' ') << "Arguments:" << std::endl;
        for (const auto& arg : arguments) {
            arg->print(indent + 4);
        }
    }
};

