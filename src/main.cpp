#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cctype>
#include "parser/parser.hpp"
#include "lexer.hpp"
#include "interpreter.hpp"

/*
    Just a few directions on how to compile:

    1. make a .ast file in the worspace folder
    2. write some codeeeee
    3. go to the main.cpp file and change the directory to the name of your file
    4. if you need documentation, there's a documentation file for you with the .md file
*/

int main() {

    std::ifstream file("workspace/example.ast");
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    std::string content;
    std::string temp_line;
    while(std::getline(file, temp_line)) {
        content += temp_line + "\n";
    }

    file.close();

    try {
        std::vector<Token> tokens = lexer(content);

        Parser parser(tokens);
        auto program = parser.parse_program();

        Interpreter interpreter;
        interpreter.execute_program(program.get());
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}