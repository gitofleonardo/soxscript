//
// Created by hhvvg on 8/18/24.
//

#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <vector>
#include "token.hpp"

class Lexer {
    unsigned int start = 0;
    unsigned int current = 0;
    unsigned int line = 1;
    unsigned int codeLength = 0;
    std::string *codes;

    std::vector<Token *> tokens;

    [[nodiscard]] bool isAtEnd() const;

    void scanToken();

    [[nodiscard]] char peekNext() const;

    [[nodiscard]] char peek() const;

    bool match(char expected);

    char advance();

    void addToken(TokenType type);

    void processIdentifier();

    void processNumberLiteral(char c);

    void processStringLiteral();

    void error(const std::string &error) const;

protected:
    const char CHAR_EOF = 0;

public:
    explicit Lexer(std::string *);

    void tokenize();

    void printTokens() const;

    [[nodiscard]] ulong tokenSize() const;

    [[nodiscard]] Token* getTokenAt(ulong index) const;

    ~Lexer();
};
#endif //LEXER_HPP
