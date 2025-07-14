#pragma once
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include "../lexer.hpp"
#include "expressions.hpp"
#include "statements.hpp"
#include "functions.hpp"

struct Parser {
    std::vector<Token> toks;
    size_t pos = 0;

    Parser(std::vector<Token> toks) : toks(toks) {}

    Token current_token() {
        if (pos >= toks.size()) {
            return Token(TokenType::SEMICOLON, "");
        }
        return toks[pos];
    }

    Token peek_token(int offset = 1) {
        if (pos + offset >= toks.size()) {
            return Token(TokenType::SEMICOLON, "");
        }
        return toks[pos + offset];
    }

    void advance() {
        if (pos < toks.size()) pos++;
    }

    bool match(TokenType type) {
        if (current_token().type == type) {
            advance();
            return true;
        }
        return false;
    }

    void expect(TokenType type) {
        if (!match(type)) {
            throw std::runtime_error("Expected token type " + std::to_string(type) +
                                   " but got " + std::to_string(current_token().type));
        }
    }

    static inline int get_binding_power(TokenType type) {
        switch(type) {
            case TokenType::EQUALS:
                return 1;
            case TokenType::PLUS:
            case TokenType::MINUS:
                return 2;
            case TokenType::STAR:
            case TokenType::SLASH:
                return 3;
            case TokenType::CARET:
                return 4;
            default:
                return 0;
        }
    }
    
    std::unique_ptr<Expression> parse_expression(int min_precedence) {
        auto left = parse_primary();

        while (pos < toks.size()) {
            Token op = current_token();
            int precedence = get_binding_power(op.type);

            if (precedence <= min_precedence) {
                break;
            }

            advance();
            auto right = parse_expression(precedence + 1);
            left = std::make_unique<BinaryExpression>(std::move(left), op.type, std::move(right));
        }

        return left;
    }
    std::unique_ptr<Expression> parse_primary() {
    Token tok = current_token();

    switch (tok.type) {
        case TokenType::INT: {
            advance();
            return std::make_unique<IntLiteral>(std::stoi(tok.value));
        }
        case TokenType::FLOAT: {
            advance();
            return std::make_unique<FloatLiteral>(std::stof(tok.value));
        }
        case TokenType::STRING: {
            advance();
            std::string str_val = tok.value;
            if (str_val.length() >= 2 && str_val[0] == '"' && str_val.back() == '"') {
                str_val = str_val.substr(1, str_val.length() - 2);
            }
            return std::make_unique<StringLiteral>(str_val);
        }
        case TokenType::BOOL: {
            advance();
            bool val = (tok.value == "true");
            return std::make_unique<BooleanLiteral>(val);
        }
        case TokenType::IDENTIFIER: {
            std::string name = tok.value;
            advance();

            if (current_token().type == TokenType::OPEN_PAREN) {
                advance();

                std::vector<std::unique_ptr<Expression>> arguments;

                if (current_token().type != TokenType::CLOSE_PAREN) {
                    do {
                        arguments.push_back(parse_expression(0));
                        if (current_token().type == TokenType::COMMA) {
                            advance();
                        } else {
                            break;
                        }
                    } while (true);
                }

                expect(TokenType::CLOSE_PAREN);
                return std::make_unique<FunctionCall>(name, std::move(arguments));
            } else {
                return std::make_unique<Identifier>(name);
            }
        }
        case TokenType::OPEN_PAREN: {
            advance();
            auto expr = parse_expression(0);
            expect(TokenType::CLOSE_PAREN);
            return std::make_unique<ParenthesizedExpression>(std::move(expr));
        }
        case TokenType::MINUS:
        case TokenType::PLUS: {
            advance();
            auto operand = parse_expression(get_binding_power(tok.type));
            return std::make_unique<UnaryExpression>(tok.type, std::move(operand));
        }
        case TokenType::PRINT:
        case TokenType::ROUND:
        case TokenType::FLOOR:
        case TokenType::CEIL:
        case TokenType::ABS:
        case TokenType::MIN:
        case TokenType::MAX:
        case TokenType::SQRT:
        case TokenType::POW: {
            std::string name = tok.value;
            advance();
            expect(TokenType::OPEN_PAREN);

            std::vector<std::unique_ptr<Expression>> arguments;

            if (current_token().type != TokenType::CLOSE_PAREN) {
                do {
                    arguments.push_back(parse_expression(0));
                    if (current_token().type == TokenType::COMMA) {
                        advance();
                    } else {
                        break;
                    }
                } while (true);
            }

            expect(TokenType::CLOSE_PAREN);
            return std::make_unique<FunctionCall>(name, std::move(arguments));
        }
        default:
            throw std::runtime_error("Unexpected token in primary expression: " + tok.value);
    }
}
    std::unique_ptr<Statement> parse_variable_declaration() {
        expect(TokenType::VAR);
        
        if (current_token().type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected identifier after 'var'");
        }
        
        std::string name = current_token().value;
        advance();
        
        std::unique_ptr<Expression> initializer = nullptr;
        if (match(TokenType::EQUALS)) {
            initializer = parse_expression(0);
        }
        
        expect(TokenType::SEMICOLON);
        return std::make_unique<VariableDeclaration>(name, std::move(initializer));
    }

    std::unique_ptr<Statement> parse_if_statement() {
        expect(TokenType::IF);
        expect(TokenType::OPEN_PAREN);
        auto condition = parse_expression(0);
        expect(TokenType::CLOSE_PAREN);
        expect(TokenType::THEN);
        
        auto then_stmt = parse_statement();
        
        std::unique_ptr<Statement> else_stmt = nullptr;
        if (match(TokenType::ELSE)) {
            else_stmt = parse_statement();
        }
        
        return std::make_unique<IfStatement>(std::move(condition), std::move(then_stmt), std::move(else_stmt));
    }
    std::unique_ptr<Statement> parse_while_statement() {
        expect(TokenType::WHILE);
        expect(TokenType::OPEN_PAREN);
        auto condition = parse_expression(0);
        expect(TokenType::CLOSE_PAREN);
        
        auto body = parse_statement();
        
        return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
    }
    std::unique_ptr<Statement> parse_function_declaration() {
        expect(TokenType::FUNC);

        if (current_token().type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected function name after 'func'");
        }
        std::string func_name = current_token().value;
        advance();

        expect(TokenType::OPEN_PAREN);
        std::vector<std::string> parameters;

        if (current_token().type != TokenType::CLOSE_PAREN) {
            do {
                if (current_token().type != TokenType::IDENTIFIER) {
                    throw std::runtime_error("Expected parameter name");
                }
                parameters.push_back(current_token().value);
                advance();

                if (current_token().type == TokenType::COMMA) {
                    advance();
                } else {
                    break;
                }
            } while (true);
        }

        expect(TokenType::CLOSE_PAREN);

        auto body = parse_statement();

        return std::make_unique<FunctionDeclaration>(func_name, std::move(parameters), std::move(body));
    }

    std::unique_ptr<Statement> parse_return_statement() {
        expect(TokenType::RET);

        std::unique_ptr<Expression> value = nullptr;
        if (current_token().type != TokenType::SEMICOLON) {
            value = parse_expression(0);
        }

        expect(TokenType::SEMICOLON);
        return std::make_unique<ReturnStatement>(std::move(value));
    }
    std::unique_ptr<Statement> parse_block_statement() {
        expect(TokenType::OPEN_CURLY);
        
        std::vector<std::unique_ptr<Statement>> statements;
        while (current_token().type != TokenType::CLOSE_CURLY && pos < toks.size()) {
            statements.push_back(parse_statement());
        }
        
        expect(TokenType::CLOSE_CURLY);
        return std::make_unique<BlockStatement>(std::move(statements));
    }
    std::unique_ptr<Statement> parse_expression_statement() {
        if (current_token().type == TokenType::IDENTIFIER && peek_token().type == TokenType::EQUALS) {
            std::string var_name = current_token().value;
            advance(); 
            advance();
            auto value = parse_expression(0);
            expect(TokenType::SEMICOLON);
            return std::make_unique<AssignmentStatement>(var_name, std::move(value));
        }

        auto expr = parse_expression(0);
        expect(TokenType::SEMICOLON);
        return std::make_unique<ExpressionStatement>(std::move(expr));
    }
    std::unique_ptr<Statement> parse_statement() {
        Token tok = current_token();

        switch (tok.type) {
            case TokenType::VAR:
                return parse_variable_declaration();
            case TokenType::FUNC:
                return parse_function_declaration();
            case TokenType::IF:
                return parse_if_statement();
            case TokenType::WHILE:
                return parse_while_statement();
            case TokenType::RET:
                return parse_return_statement();
            case TokenType::OPEN_CURLY:
                return parse_block_statement();
            default:
                return parse_expression_statement();
        }
    }

    std::unique_ptr<Program> parse_program() {
        std::vector<std::unique_ptr<Statement>> statements;
        
        while (pos < toks.size()) {
            if (match(TokenType::SEMICOLON)) {
                continue;
            }
            
            statements.push_back(parse_statement());
        }
        
        return std::make_unique<Program>(std::move(statements));
    }

};