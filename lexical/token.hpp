//
// Created by hhvvg on 8/18/24.
//

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <vector>
#include <string>
#include "token_type.hpp"

class Token {
    TokenType _type;
    std::string _lexeme;
    uint _line;

public:
    Token(const TokenType type, std::string lexeme, const uint line): _type(type),
                                                                      _lexeme(std::move(lexeme)), _line(line) {
    }

    virtual ~Token() = default;

    [[nodiscard]] TokenType type() const {
        return _type;
    }

    [[nodiscard]] const std::string &lexeme() const {
        return _lexeme;
    }

    [[nodiscard]] uint line() const {
        return _line;
    }

    [[nodiscard]] std::string toString() const {
        if (_type == FILE_EOF) {
            return "Token(EOF)";
        }
        return std::string("Token(") + std::to_string(_type) + "," + _lexeme + ", " + std::to_string(_line) + ")";
    }
};

class StringToken final : public Token {
public:
    std::vector<Token *> tokens;

    StringToken(const std::string &lexeme, const uint line,
                std::vector<Token *> tokens): Token(STRING, lexeme, line), tokens(std::move(tokens)) {
    }

    ~StringToken() override {
        for (const auto token : tokens) {
            delete token;
        }
    }
};

#endif //TOKEN_HPP
