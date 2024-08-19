//
// Created by hhvvg on 8/18/24.
//

#include "lexer.hpp"

#include "../utils/logger.hpp"
#include "token_type.hpp"
#include "../utils/utils.hpp"

Lexer::Lexer(std::string *codes): codes(codes) {
    codeLength = codes->size();
}

void Lexer::tokenize() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back(new Token(FILE_EOF, nullptr, line));
}

Lexer::~Lexer() {
    delete codes;
    for (const auto &token: tokens) {
        delete token;
    }
}

bool Lexer::isAtEnd() const {
    return current >= codeLength;
}

void Lexer::scanToken() {
    if (const char c = advance(); c == '(') {
        addToken(L_PAREN);
    } else if (c == ')') {
        addToken(R_PAREN);
    } else if (c == '{') {
        addToken(L_BRACE);
    } else if (c == '}') {
        addToken(R_BRACE);
    } else if (c == ',') {
        addToken(COMMA);
    } else if (c == '.') {
        addToken(DOT);
    } else if (c == ':') {
        addToken(COLON);
    } else if (c == ';') {
        addToken(SEMICOLON);
    } else if (c == '+') {
        addToken(PLUS);
    } else if (c == '-') {
        addToken(MINUS);
    } else if (c == '*') {
        addToken(STAR);
    } else if (c == '?') {
        addToken(QUESTION_MARK);
    } else if (c == '!') {
        addToken(match('=') ? BANG_EQUAL : BANG);
    } else if (c == '=') {
        addToken(match('=') ? EQUAL_EQUAL : EQUAL);
    } else if (c == '>') {
        addToken(match('=') ? GREATER_EQUAL : GREATER);
    } else if (c == '<') {
        addToken(match('=') ? LESS_EQUAL : LESS);
    } else if (c == '|') {
        if (match('|')) {
            addToken(OR);
        } else {
            addToken(VERTICAL_BAR);
        }
    } else if (c == '&') {
        if (match('&')) {
            addToken(AND);
        } else {
            error("Unexpected character '&'");
        }
    } else if (c == '/') {
        addToken(SLASH);
    } else if (c == '\\') {
        addToken(BACKSLASH);
    } else if (c == '#') {
        while (peek() != '\n' && !isAtEnd()) advance();
    } else if (c == '"') {
        processStringLiteral();
    } else if (isLegalIdentifierPrefix(c)) {
        processIdentifier();
    } else if (isDigit(c)) {
        processNumberLiteral(c);
    }
}

char Lexer::peekNext() const {
    if (current + 1 >= codeLength) {
        return CHAR_EOF;
    }
    return codes->at(current + 1);
}

char Lexer::peek() const {
    if (isAtEnd()) return CHAR_EOF;
    return codes->at(current);
}

bool Lexer::match(const char expected) {
    if (isAtEnd()) return false;
    if (codes->at(current) != expected) return false;
    advance();
    return true;
}

char Lexer::advance() {
    ++current;
    const char c = codes->at(current - 1);
    if (c == '\n') {
        ++line;
    }
    return c;
}

void Lexer::addToken(const TokenType type) {
    const auto code = new std::string(codes->substr(start, current - start));
    tokens.push_back(new Token(type, code, line));
}

void Lexer::processIdentifier() {
    while (isLegalIdentifierChar(peek())) advance();
    const auto lexeme = codes->substr(start, current - start);
    const auto type = SoxKeywords::instance()->getKeyword(lexeme);
    addToken(type);
}

void Lexer::processNumberLiteral(const char c) {
    if (c != '0') {
        // decimal
        bool isDouble = false;
        while (isDigit(peek())) advance();
        if (peek() == '.' && isDigit(peekNext())) {
            isDouble = true;
            advance();
            while (isDigit(peek())) advance();
        }
        addToken(isDouble ? DOUBLE : INT);
    } else {
        if (peek() == 'x') {
            // hex
            advance();
            if (isLegalHexNumber(peek())) {
                while (isLegalHexNumber(peek())) advance();
            } else {
                error("Unexpected hexadecimal character");
            }
        } else if (peek() == 'b') {
            advance();
            if (isLegalBinaryNumber(peek())) {
                while (isLegalBinaryNumber(peek())) advance();
            }
        } else if (isLegalOctalNumber(peek())) {
            while (isLegalOctalNumber(peek())) advance();
        }
        const auto lexeme = codes->substr(start, current - start);
        addToken(INT);
    }
}

void Lexer::processStringLiteral() {
    while (peek() != '"' && !isAtEnd()) advance();
    if (isAtEnd()) {
        error("Unterminated string literal");
        return;
    }
    advance();
    auto stringLiteral = codes->substr(start + 1, current - start - 1);
    addToken(STRING);
}

void Lexer::error(const std::string &error) const {
    Logger::instance()->logError(line, error);
}

void Lexer::printTokens() const {
    for (const auto &token : tokens) {
        std::cout << token->toString() << std::endl;
    }
}

ulong Lexer::tokenSize() const {
    return tokens.size();
}

Token* Lexer::getTokenAt(const ulong index) const {
    return tokens.at(index);
}
