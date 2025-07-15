#pragma once
#include <unordered_map>
#include <functional>
#include <vector>
#include <variant>
#include <cmath>
#include <iostream>
#include <memory>

// Forward declaration
struct Value;

// Define array type
struct ValueArray {
    std::vector<Value> elements;

    ValueArray() = default;
    ValueArray(std::vector<Value> elems) : elements(std::move(elems)) {}

    size_t size() const { return elements.size(); }
    bool empty() const { return elements.empty(); }
    Value& operator[](size_t index) { return elements[index]; }
    const Value& operator[](size_t index) const { return elements[index]; }

    auto begin() { return elements.begin(); }
    auto end() { return elements.end(); }
    auto begin() const { return elements.begin(); }
    auto end() const { return elements.end(); }
};

// Now define Value using the array struct
struct Value {
    std::variant<int, float, std::string, bool, ValueArray> data;

    Value() : data(0) {}
    Value(int v) : data(v) {}
    Value(float v) : data(v) {}
    Value(const std::string& v) : data(v) {}
    Value(bool v) : data(v) {}
    Value(const ValueArray& v) : data(v) {}
    Value(ValueArray&& v) : data(std::move(v)) {}

    template<typename T>
    bool holds() const { return std::holds_alternative<T>(data); }

    template<typename T>
    T& get() { return std::get<T>(data); }

    template<typename T>
    const T& get() const { return std::get<T>(data); }

    template<typename T>
    T* get_if() { return std::get_if<T>(&data); }

    template<typename T>
    const T* get_if() const { return std::get_if<T>(&data); }
};

using ValueVector = std::vector<Value>;

std::string value_to_string(const Value& val) {
    return std::visit([](const auto& v) -> std::string {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
            return "\"" + v + "\"";
        } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, ValueArray>) {
            std::string result = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) result += ", ";
                result += value_to_string(v[i]);
            }
            result += "]";
            return result;
        } else {
            return std::to_string(v);
        }
    }, val.data);
}

using BuiltinFunction = std::function<Value(const ValueVector&)>;

class FunctionRegistry {
private:
    std::unordered_map<std::string, BuiltinFunction> builtin_functions;

public:
    FunctionRegistry() {
        register_builtin_functions();
    }

    void register_builtin_functions() {
        builtin_functions["print"] = [](const ValueVector& args) -> Value {
            if (args.size() != 1) {
                throw std::runtime_error("print() expects exactly 1 argument");
            }
            std::cout << value_to_string(args[0]) << std::endl;
            return 0;
        };

        builtin_functions["round"] = [](const ValueVector& args) -> Value {
            if (args.size() != 1) {
                throw std::runtime_error("round() expects exactly 1 argument");
            }
            return std::visit([](const auto& val) -> Value {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<int>(std::round(val));
                } else {
                    throw std::runtime_error("round() requires numeric argument");
                }
            }, args[0].data);
        };

        builtin_functions["floor"] = [](const ValueVector& args) -> Value {
            if (args.size() != 1) {
                throw std::runtime_error("floor() expects exactly 1 argument");
            }
            return std::visit([](const auto& val) -> Value {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<int>(std::floor(val));
                } else {
                    throw std::runtime_error("floor() requires numeric argument");
                }
            }, args[0].data);
        };

        builtin_functions["ceil"] = [](const ValueVector& args) -> Value {
            if (args.size() != 1) {
                throw std::runtime_error("ceil() expects exactly 1 argument");
            }
            return std::visit([](const auto& val) -> Value {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<int>(std::ceil(val));
                } else {
                    throw std::runtime_error("ceil() requires numeric argument");
                }
            }, args[0].data);
        };

        builtin_functions["abs"] = [](const ValueVector& args) -> Value {
            if (args.size() != 1) {
                throw std::runtime_error("abs() expects exactly 1 argument");
            }
            return std::visit([](const auto& val) -> Value {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return std::abs(val);
                } else {
                    throw std::runtime_error("abs() requires numeric argument");
                }
            }, args[0].data);
        };

        builtin_functions["min"] = [](const ValueVector& args) -> Value {
            if (args.size() != 2) {
                throw std::runtime_error("min() expects exactly 2 arguments");
            }

            float a = std::visit([](const auto& val) -> float {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<float>(val);
                } else {
                    throw std::runtime_error("min() requires numeric arguments");
                }
            }, args[0].data);

            float b = std::visit([](const auto& val) -> float {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<float>(val);
                } else {
                    throw std::runtime_error("min() requires numeric arguments");
                }
            }, args[1].data);

            return std::min(a, b);
        };

        builtin_functions["max"] = [](const ValueVector& args) -> Value {
            if (args.size() != 2) {
                throw std::runtime_error("max() expects exactly 2 arguments");
            }

            float a = std::visit([](const auto& val) -> float {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<float>(val);
                } else {
                    throw std::runtime_error("max() requires numeric arguments");
                }
            }, args[0].data);

            float b = std::visit([](const auto& val) -> float {
                if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
                    return static_cast<float>(val);
                } else {
                    throw std::runtime_error("max() requires numeric arguments");
                }
            }, args[1].data);

            return std::max(a, b);
        };

        builtin_functions["sqrt"] = [](const ValueVector& args) -> Value {
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
            }, args[0].data);
        };

        builtin_functions["pow"] = [](const ValueVector& args) -> Value {
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
            }, args[0].data, args[1].data);
        };

        builtin_functions["len"] = [](const ValueVector& args) -> Value {
            if (args.size() != 1) {
                throw std::runtime_error("length() expects exactly 1 argument");
            }
            if (auto array = args[0].get_if<ValueArray>()) {
                return static_cast<int>(array->size());
            } else if (auto str = args[0].get_if<std::string>()) {
                return static_cast<int>(str->size());
            } else {
                throw std::runtime_error("length() requires array or string argument");
            }
        };
    
        builtin_functions["frag"] = [](const ValueVector& args) -> Value{
            if (args.size() != 3){
                throw std::runtime_error("frag() expects exactly 3 arguments");
            }

            if(!args[0].get_if<ValueArray>()){
                throw std::runtime_error("frag() requires array as first argument");
            }

            if(!args[1].get_if<int>() || !args[2].get_if<int>()){
                throw std::runtime_error("frag() requires integers as second and third arguments");
            }

            int start = args[1].get<int>();
            int end = args[2].get<int>();

            if(start < 0 || end < 0 || start >= end || end > args[0].get<ValueArray>().size()){
                throw std::runtime_error("frag() requires valid start and end indices");
            }

            return ValueArray(std::vector<Value>(args[0].get<ValueArray>().begin() + start, args[0].get<ValueArray>().begin() + end));
        };
    }

    Value call_function(const std::string& name, const ValueVector& args) {
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
