#pragma once
#include <vector>
#include <string>
#include <memory>

struct Expression;
struct Statement;

struct ExpressionStatement : public Statement {
    std::unique_ptr<Expression> expression;

    ExpressionStatement(std::unique_ptr<Expression> expr) : expression(std::move(expr)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "ExpressionStatement:" << std::endl;
        expression->print(indent + 2);
    }
};

struct VariableDeclaration : public Statement {
    std::string name;
    std::unique_ptr<Expression> initializer;

    VariableDeclaration(const std::string& n, std::unique_ptr<Expression> init = nullptr)
        : name(n), initializer(std::move(init)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "VariableDeclaration:" << std::endl;
        std::cout << std::string(indent + 2, ' ') << "Name: " << name << std::endl;
        if (initializer) {
            std::cout << std::string(indent + 2, ' ') << "Initializer:" << std::endl;
            initializer->print(indent + 4);
        }
    }
};
struct AssignmentStatement : public Statement {
    std::string variable_name;
    std::unique_ptr<Expression> value;

    AssignmentStatement(const std::string& name, std::unique_ptr<Expression> val)
        : variable_name(name), value(std::move(val)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "AssignmentStatement:" << std::endl;
        std::cout << std::string(indent + 2, ' ') << "Variable: " << variable_name << std::endl;
        std::cout << std::string(indent + 2, ' ') << "Value:" << std::endl;
        value->print(indent + 4);
    }
};
struct ReturnStatement : public Statement {
    std::unique_ptr<Expression> value;

    ReturnStatement(std::unique_ptr<Expression> val = nullptr) : value(std::move(val)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "ReturnStatement:" << std::endl;
        if (value) {
            std::cout << std::string(indent + 2, ' ') << "Value:" << std::endl;
            value->print(indent + 4);
        }
    }
};

struct FunctionDeclaration : public Statement {
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<Statement> body;

    FunctionDeclaration(const std::string& func_name,
                       std::vector<std::string> params,
                       std::unique_ptr<Statement> func_body)
        : name(func_name), parameters(std::move(params)), body(std::move(func_body)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "FunctionDeclaration: " << name << std::endl;
        std::cout << std::string(indent + 2, ' ') << "Parameters: ";
        for (size_t i = 0; i < parameters.size(); ++i) {
            std::cout << parameters[i];
            if (i < parameters.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
        std::cout << std::string(indent + 2, ' ') << "Body:" << std::endl;
        body->print(indent + 4);
    }
};

struct IfStatement : public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> then_statement;
    std::unique_ptr<Statement> else_statement;

    IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> then_stmt,
                std::unique_ptr<Statement> else_stmt = nullptr)
        : condition(std::move(cond)), then_statement(std::move(then_stmt)),
          else_statement(std::move(else_stmt)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "IfStatement:" << std::endl;
        std::cout << std::string(indent + 2, ' ') << "Condition:" << std::endl;
        condition->print(indent + 4);
        std::cout << std::string(indent + 2, ' ') << "Then:" << std::endl;
        then_statement->print(indent + 4);
        if (else_statement) {
            std::cout << std::string(indent + 2, ' ') << "Else:" << std::endl;
            else_statement->print(indent + 4);
        }
    }
};
struct WhileStatement : public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    WhileStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> body_stmt)
        : condition(std::move(cond)), body(std::move(body_stmt)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "WhileStatement:" << std::endl;
        std::cout << std::string(indent + 2, ' ') << "Condition:" << std::endl;
        condition->print(indent + 4);
        std::cout << std::string(indent + 2, ' ') << "Body:" << std::endl;
        body->print(indent + 4);
    }
};
struct BlockStatement : public Statement {
    std::vector<std::unique_ptr<Statement>> statements;

    BlockStatement(std::vector<std::unique_ptr<Statement>> stmts) : statements(std::move(stmts)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "BlockStatement:" << std::endl;
        for (const auto& stmt : statements) {
            stmt->print(indent + 2);
        }
    }
};

struct Program : public ASTNode {
    std::vector<std::unique_ptr<Statement>> statements;

    Program(std::vector<std::unique_ptr<Statement>> stmts) : statements(std::move(stmts)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Program:" << std::endl;
        for (const auto& stmt : statements) {
            stmt->print(indent + 2);
        }
    }
};