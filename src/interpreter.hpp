#pragma once
#include <unordered_map>
#include <variant>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include "parser/expressions.hpp"
#include "parser/statements.hpp"
#include "parser/functions.hpp"
#include <math.h>

struct ReturnException {
    Value value;
    ReturnException(const Value& val) : value(val) {}
};

struct UserFunction {
    std::vector<std::string> parameters;
    const Statement* body;

    UserFunction(std::vector<std::string> params, const Statement* func_body)
        : parameters(std::move(params)), body(func_body) {}
};

class Interpreter {
private:
    std::unordered_map<std::string, Value> variables;
    std::unordered_map<std::string, UserFunction> user_functions;
    FunctionRegistry function_registry;

public:
    Value evaluate_expression(const Expression* expr) {
        if (auto int_lit = dynamic_cast<const IntLiteral*>(expr)) return int_lit->value;
        else if (auto float_lit = dynamic_cast<const FloatLiteral*>(expr)) return float_lit->value;
        else if (auto string_lit = dynamic_cast<const StringLiteral*>(expr)) return string_lit->value;
        else if (auto bool_lit = dynamic_cast<const BooleanLiteral*>(expr)) return bool_lit->value;
        else if (auto identifier = dynamic_cast<const Identifier*>(expr)) {
            auto it = variables.find(identifier->name);
            if (it == variables.end()) {
                throw std::runtime_error("Undefined variable: " + identifier->name);
            }
            return it->second;
        }
        else if (auto binary_expr = dynamic_cast<const BinaryExpression*>(expr)) return evaluate_binary_expression(binary_expr);
        else if (auto unary_expr = dynamic_cast<const UnaryExpression*>(expr)) return evaluate_unary_expression(unary_expr);
        else if (auto paren_expr = dynamic_cast<const ParenthesizedExpression*>(expr)) return evaluate_expression(paren_expr->expression.get());
        else if (auto func_call = dynamic_cast<const FunctionCall*>(expr)) return evaluate_function_call(func_call);
        else if (auto room_lit = dynamic_cast<const RoomLiteral*>(expr)) {
            ValueVector elements;
            for (const auto& element : room_lit->elements) {
                elements.push_back(evaluate_expression(element.get()));
            }
            return Value(ValueArray(std::move(elements)));
        }
        else if(auto room_access = dynamic_cast<const RoomAccess*>(expr)){

            if(variables.find(room_access->room_name) == variables.end()){
                throw std::runtime_error("Undefined room: " + room_access->room_name);
            }
            
            if(!variables[room_access->room_name].holds<ValueArray>()){
                throw std::runtime_error("Not a room: " + room_access->room_name);
            }

            Value index_val = evaluate_expression(room_access->index.get());
            int index = std::visit([](const auto& val) -> int {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<int>(val);
                } else {
                    throw std::runtime_error("room index must be numeric");
                }
            }, index_val.data);

            ValueArray& room = variables[room_access->room_name].get<ValueArray>();
            if (index < 0 || index >= static_cast<int>(room.size())) {
                throw std::runtime_error("room index out of bounds");
            }
            return room[index];
        }

        else throw std::runtime_error("Unknown expression type");
    }

    Value evaluate_binary_expression(const BinaryExpression* expr) {
        Value left = evaluate_expression(expr->left.get());
        Value right = evaluate_expression(expr->right.get());

        switch (expr->operator_type) {
            case TokenType::PLUS:
                return std::visit([](const auto& l, const auto& r) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(l)>> &&
                                  std::is_arithmetic_v<std::decay_t<decltype(r)>>) {
                        return l + r;
                    } else {
                        throw std::runtime_error("Invalid operands for +");
                    }
                }, left.data, right.data);
            
            case TokenType::MINUS:
                return std::visit([](const auto& l, const auto& r) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(l)>> &&
                                  std::is_arithmetic_v<std::decay_t<decltype(r)>>) {
                        return l - r;
                    } else {
                        throw std::runtime_error("Invalid operands for -");
                    }
                }, left.data, right.data);
            
            case TokenType::STAR:
                return std::visit([](const auto& l, const auto& r) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(l)>> &&
                                  std::is_arithmetic_v<std::decay_t<decltype(r)>>) {
                        return l * r;
                    } else {
                        throw std::runtime_error("Invalid operands for *");
                    }
                }, left.data, right.data);
            
            case TokenType::SLASH:
                return std::visit([](const auto& l, const auto& r) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(l)>> &&
                                  std::is_arithmetic_v<std::decay_t<decltype(r)>>) {
                        if (r == 0) throw std::runtime_error("Division by zero");
                        return l / r;
                    } else {
                        throw std::runtime_error("Invalid operands for /");
                    }
                }, left.data, right.data);

            case TokenType::CARET:
                return std::visit([](const auto& l, const auto& r) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(l)>> &&
                                  std::is_arithmetic_v<std::decay_t<decltype(r)>>) {
                        return static_cast<float>(std::pow(l, r));
                    } else {
                        throw std::runtime_error("Invalid operands for ^");
                    }
                }, left.data, right.data);

            case TokenType::EQUALS:
                throw std::runtime_error("Assignment not supported in expressions");
            
            default:
                throw std::runtime_error("Unknown binary operator");
        }
    }

    Value evaluate_unary_expression(const UnaryExpression* expr) {
        Value operand = evaluate_expression(expr->operand.get());

        switch (expr->operator_type) {
            case TokenType::MINUS:
                return std::visit([](const auto& val) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                        return -val;
                    } else {
                        throw std::runtime_error("Invalid operand for unary -");
                    }
                }, operand.data);
            
            case TokenType::PLUS:
                return std::visit([](const auto& val) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                        return +val;
                    } else {
                        throw std::runtime_error("Invalid operand for unary +");
                    }
                }, operand.data);
            
            default:
                throw std::runtime_error("Unknown unary operator");
        }
    }

    Value evaluate_function_call(const FunctionCall* func_call) {
        ValueVector args;
        for (const auto& arg_expr : func_call->arguments) {
            args.push_back(evaluate_expression(arg_expr.get()));
        }

        auto user_func_it = user_functions.find(func_call->function_name);
        if (user_func_it != user_functions.end()) {
            return call_user_function(user_func_it->second, args);
        }

        return function_registry.call_function(func_call->function_name, args);
    }

    Value call_user_function(const UserFunction& func, const ValueVector& args) {
        if (args.size() != func.parameters.size()) {
            throw std::runtime_error("Function expects " + std::to_string(func.parameters.size()) +
                                   " arguments, got " + std::to_string(args.size()));
        }

        auto saved_variables = variables;

        for (size_t i = 0; i < func.parameters.size(); ++i) {
            variables[func.parameters[i]] = args[i];
        }

        Value return_value = 0;

        try {
            execute_statement(func.body);
        } catch (const ReturnException& ret) {
            return_value = ret.value;
        }

        variables = saved_variables;

        return return_value;
    }

    void execute_statement(const Statement* stmt) {
        if (auto var_decl = dynamic_cast<const VariableDeclaration*>(stmt)) {
            Value value = 0;
            if (var_decl->initializer) {
                value = evaluate_expression(var_decl->initializer.get());
            }
            variables[var_decl->name] = value;
        }
        else if (auto func_decl = dynamic_cast<const FunctionDeclaration*>(stmt)) {
            user_functions.emplace(func_decl->name, UserFunction(func_decl->parameters, func_decl->body.get()));
        }
        else if (auto assign_stmt = dynamic_cast<const AssignmentStatement*>(stmt)) {
            Value value = evaluate_expression(assign_stmt->value.get());
            variables[assign_stmt->variable_name] = value;
        }
        else if (auto expr_stmt = dynamic_cast<const ExpressionStatement*>(stmt)) {
            Value result = evaluate_expression(expr_stmt->expression.get());
        }
        else if (auto if_stmt = dynamic_cast<const IfStatement*>(stmt)) {
            Value condition = evaluate_expression(if_stmt->condition.get());
            bool is_true = std::visit([](const auto& val) -> bool {
                if constexpr (std::is_same_v<std::decay_t<decltype(val)>, bool>) {
                    return val;
                } else if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return val != 0;
                } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
                    return !val.empty();
                } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, ValueArray>) {
                    return !val.empty();
                } else {
                    return false;
                }
            }, condition.data);

            if (is_true) {
                execute_statement(if_stmt->then_statement.get());
            } else if (if_stmt->else_statement) {
                execute_statement(if_stmt->else_statement.get());
            }
        }
        else if (auto block_stmt = dynamic_cast<const BlockStatement*>(stmt)) {
            for (const auto& statement : block_stmt->statements) {
                execute_statement(statement.get());
            }
        }
        else if (auto ret_stmt = dynamic_cast<const ReturnStatement*>(stmt)) {
            Value return_value = 0; 
            if (ret_stmt->value) {
                return_value = evaluate_expression(ret_stmt->value.get());
            }
            throw ReturnException(return_value);
        }
        else if (auto room_assign = dynamic_cast<const RoomAssignmentStatement*>(stmt)){
            if (variables.find(room_assign->room_name) == variables.end()) {
                throw std::runtime_error("Undefined room: " + room_assign->room_name);
            }

            Value index_val = evaluate_expression(room_assign->index.get());
            int index = std::visit([](const auto& val) -> int {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<int>(val);
                } else {
                    throw std::runtime_error("Room index must be numeric");
                }
            }, index_val.data);

            Value new_value = evaluate_expression(room_assign->value.get());

            if (!variables[room_assign->room_name].holds<ValueArray>()) {
                throw std::runtime_error("Variable is not a room: " + room_assign->room_name);
            }

            ValueArray& room = variables[room_assign->room_name].get<ValueArray>();
            if (index < 0 || index >= static_cast<int>(room.size())) {
                throw std::runtime_error("Room index out of bounds");
            }
            room[index] = new_value;
        }
        else {
            throw std::runtime_error("Unknown statement type");
        }
    }

    void execute_program(const Program* program) {
        try {
            for (const auto& statement : program->statements) {
                execute_statement(statement.get());
            }
        } catch (const ReturnException& ret) {
            std::cout << "Program exited with return value: " << value_to_string(ret.value) << std::endl;
        }
    }
    void print_variables() {
        std::cout << "\n=== VARIABLES ===" << std::endl;
        for (const auto& [name, value] : variables) {
            std::cout << name << " = " << value_to_string(value) << std::endl;
        }
    }
};
