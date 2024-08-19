#include <fstream>
#include <iostream>

#include "interpret/interpreter.hpp"
#include "interpret/resolver.hpp"
#include "parser/parser.hpp"
#include "lexical/lexer.hpp"

int runFile(const std::string& fileName);
int runPrompt();
int runCodes(std::string *codes);

int main(const int argc, const char *argv[]) {
    if (argc > 2) {
        std::cout << "Usage: soxsh [script].";
        return 0;
    }
    if (argc == 2) {
        return runFile(std::string(argv[1]));
    }
    return runPrompt();
}

int runFile(const std::string& fileName) {
    std::fstream fs(fileName, std::ios::in);
    auto *str = new std::string((std::istreambuf_iterator(fs)), std::istreambuf_iterator<char>());
    return runCodes(str);
}

int runPrompt() {
    std::string line;
    while (std::getline(std::cin, line)) {
        runCodes(new std::string(line));
    }
    return 0;
}

int runCodes(std::string *codes) {
    Lexer l(codes);
    l.tokenize();
    l.printTokens();
    Parser p(&l);
    const auto stmts = p.parse();
    Interpreter interpreter;
    Resolver resolver(&interpreter);
    resolver.resolve(stmts);
    interpreter.interpret(stmts);
    for (const auto stmt : *stmts) {
        delete stmt;
    }
    return 0;
}
