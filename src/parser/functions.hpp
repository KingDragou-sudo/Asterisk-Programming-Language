#pragma once
#include <unordered_map>
#include <functional>
#include <vector>
#include <variant>
#include <cmath>
#include <iostream>

using Value = std::variant<int, float, std::string, bool>;

std::string value_to_string(const Value& val) {
    return std::visit([](const auto& v) -> std::string {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
            return "\"" + v + "\"";
        } else {
            return std::to_string(v);
        }
    }, val);
}

using BuiltinFunction = std::function<Value(const std::vector<Value>&)>;

class FunctionRegistry {
private:
    std::unordered_map<std::string, BuiltinFunction> builtin_functions;

public:
    FunctionRegistry() {
        register_builtin_functions();
    }

    void register_builtin_functions() {
        builtin_functions["print"] = [](const std::vector<Value>& args) -> Value {
            if (args.size() != 1) {
                throw std::runtime_error("print() expects exactly 1 argument");
            }
            std::cout << value_to_string(args[0]) << std::endl;
            return 0;
        };

        builtin_functions["round"] = [](const std::vector<Value>& args) -> Value {
            if (args.size() != 1) {
                throw std::runtime_error("round() expects exactly 1 argument");
            }
            return std::visit([](const auto& val) -> Value {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<int>(std::round(val));
                } else {
                    throw std::runtime_error("round() requires numeric argument");
                }
            }, args[0]);
        };

        builtin_functions["floor"] = [](const std::vector<Value>& args) -> Value {
            if (args.size() != 1) {
                throw std::runtime_error("floor() expects exactly 1 argument");
            }
            return std::visit([](const auto& val) -> Value {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<int>(std::floor(val));
                } else {
                    throw std::runtime_error("floor() requires numeric argument");
                }
            }, args[0]);
        };

        builtin_functions["ceil"] = [](const std::vector<Value>& args) -> Value {
            if (args.size() != 1) {
                throw std::runtime_error("ceil() expects exactly 1 argument");
            }
            return std::visit([](const auto& val) -> Value {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<int>(std::ceil(val));
                } else {
                    throw std::runtime_error("ceil() requires numeric argument");
                }
            }, args[0]);
        };

        builtin_functions["abs"] = [](const std::vector<Value>& args) -> Value {
            if (args.size() != 1) {
                throw std::runtime_error("abs() expects exactly 1 argument");
            }
            return std::visit([](const auto& val) -> Value {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return std::abs(val);
                } else {
                    throw std::runtime_error("abs() requires numeric argument");
                }
            }, args[0]);
        };

        builtin_functions["min"] = [](const std::vector<Value>& args) -> Value {
            if (args.size() != 2) {
                throw std::runtime_error("min() expects exactly 2 arguments");
            }

            float a = std::visit([](const auto& val) -> float {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<float>(val);
                } else {
                    throw std::runtime_error("min() requires numeric arguments");
                }
            }, args[0]);

            float b = std::visit([](const auto& val) -> float {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<float>(val);
                } else {
                    throw std::runtime_error("min() requires numeric arguments");
                }
            }, args[1]);

            return std::min(a, b);
        };

        builtin_functions["max"] = [](const std::vector<Value>& args) -> Value {
            if (args.size() != 2) {
                throw std::runtime_error("max() expects exactly 2 arguments");
            }

            float a = std::visit([](const auto& val) -> float {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<float>(val);
                } else {
                    throw std::runtime_error("max() requires numeric arguments");
                }
            }, args[0]);

            float b = std::visit([](const auto& val) -> float {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<float>(val);
                } else {
                    throw std::runtime_error("max() requires numeric arguments");
                }
            }, args[1]);

            return std::max(a, b);
        };

        builtin_functions["sqrt"] = [](const std::vector<Value>& args) -> Value {
            if (args.size() != 1) {
                throw std::runtime_error("sqrt() expects exactly 1 argument");
            }
            return std::visit([](const auto& val) -> Value {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    if (val < 0) {
                        throw std::runtime_error("sqrt() requires non-negative argument");
                    }
                    return static_cast<float>(std::sqrt(val));
                } else {
                    throw std::runtime_error("sqrt() requires numeric argument");
                }
            }, args[0]);
        };

        builtin_functions["pow"] = [](const std::vector<Value>& args) -> Value {
            if (args.size() != 2) {
                throw std::runtime_error("pow() expects exactly 2 arguments");
            }
            return std::visit([](const auto& base, const auto& exp) -> Value {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(base)>> &&
                              std::is_arithmetic_v<std::decay_t<decltype(exp)>>) {
                    return static_cast<float>(std::pow(base, exp));
                } else {
                    throw std::runtime_error("pow() requires numeric arguments");
                }
            }, args[0], args[1]);
        };
    }

    Value call_function(const std::string& name, const std::vector<Value>& args) {
        auto it = builtin_functions.find(name);
        if (it != builtin_functions.end()) {
            return it->second(args);
        }
        throw std::runtime_error("Unknown function: " + name);
    }
    bool function_exists(const std::string& name) const {
        return builtin_functions.find(name) != builtin_functions.end();
    }

    void register_function(const std::string& name, BuiltinFunction func) {
        builtin_functions[name] = func;
    }

    std::vector<std::string> get_function_names() const {
        std::vector<std::string> names;
        for (const auto& pair : builtin_functions) {
            names.push_back(pair.first);
        }
        return names;
    }
};
