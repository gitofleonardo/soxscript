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
    tokens.push_back(new Token(FILE_EOF, "", line));
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
        addToken(match('+') ? PLUS_PLUS : PLUS);
    } else if (c == '-') {
        addToken(match('-') ? MINUS_MINUS : MINUS);
    } else if (c == '*') {
        addToken(STAR);
    } else if (c == '?') {
        addToken(QUESTION_MARK);
    } else if (c == '[') {
        addToken(L_BRACKET);
    } else if (c == ']') {
        addToken(R_BRACKET);
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
        processIdentifier(tokens);
    } else if (isDigit(c)) {
        processNumberLiteral(c, tokens);
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

void Lexer::addToken(std::vector<Token *> &tokens, const TokenType type, const uint start, const uint len) const {
    const auto code = codes->substr(start, len);
    tokens.push_back(new Token(type, code, line));
}

void Lexer::addToken(std::vector<Token *> &tokens, const TokenType type) const {
    addToken(tokens, type, start, current - start);
}

void Lexer::addToken(const TokenType type) {
    addToken(tokens, type);
}

void Lexer::processIdentifier(std::vector<Token *> &tokens) {
    while (isLegalIdentifierChar(peek())) advance();
    const auto lexeme = codes->substr(start, current - start);
    const auto type = SoxKeywords::instance()->getKeyword(lexeme);
    addToken(tokens, type);
}

void Lexer::processNumberLiteral(const char c, std::vector<Token *> &tokens) {
    if (c != '0') {
        // decimal
        bool isDouble = false;
        while (isDigit(peek())) advance();
        if (peek() == '.' && isDigit(peekNext())) {
            isDouble = true;
            advance();
            while (isDigit(peek())) advance();
        }
        addToken(tokens, isDouble ? DOUBLE : INT);
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
        addToken(tokens, INT);
    }
}

void Lexer::processStringLiteral() {
    const auto originalStart = start;
    // We don't need a '"' in string, so move start to current
    start = current;
    std::vector<Token *> stringTokens;
    while (peek() != '"' && !isAtEnd()) {
        if (const auto c = advance(); c == '$') {
            // Minus 1, so that the '$' won't be added in string
            addToken(stringTokens, STRING, start, current - start - 1);
            if (match('{')) {
                // Process expr
                while (peek() != '}' && !isAtEnd()) {
                    start = current;
                    if (const char ch = advance(); ch == '(') {
                        addToken(stringTokens, L_PAREN);
                    } else if (ch == ')') {
                        addToken(stringTokens, R_PAREN);
                    } else if (ch == ',') {
                        addToken(stringTokens, COMMA);
                    } else if (ch == '.') {
                        addToken(stringTokens, DOT);
                    } else if (ch == ':') {
                        addToken(stringTokens, COLON);
                    } else if (ch == '+') {
                        addToken(stringTokens, match('+') ? PLUS_PLUS : PLUS);
                    } else if (ch == '-') {
                        addToken(stringTokens, match('-') ? MINUS_MINUS : MINUS);
                    } else if (ch == '*') {
                        addToken(stringTokens, STAR);
                    } else if (ch == '?') {
                        addToken(stringTokens, QUESTION_MARK);
                    } else if (ch == '[') {
                        addToken(stringTokens, L_BRACKET);
                    } else if (ch == ']') {
                        addToken(stringTokens, R_BRACKET);
                    } else if (ch == '!') {
                        addToken(stringTokens, match('=') ? BANG_EQUAL : BANG);
                    } else if (ch == '=') {
                        addToken(stringTokens, match('=') ? EQUAL_EQUAL : EQUAL);
                    } else if (ch == '>') {
                        addToken(stringTokens, match('=') ? GREATER_EQUAL : GREATER);
                    } else if (ch == '<') {
                        addToken(stringTokens, match('=') ? LESS_EQUAL : LESS);
                    } else if (ch == '|') {
                        if (match('|')) {
                            addToken(stringTokens, OR);
                        } else {
                            addToken(stringTokens, VERTICAL_BAR);
                        }
                    } else if (ch == '&') {
                        if (match('&')) {
                            addToken(stringTokens, AND);
                        } else {
                            error("Unexpected character '&'");
                        }
                    } else if (ch == '/') {
                        addToken(stringTokens, SLASH);
                    } else if (isLegalIdentifierPrefix(ch)) {
                        processIdentifier(stringTokens);
                    } else if (isDigit(ch)) {
                        processNumberLiteral(ch, stringTokens);
                    } else {
                        error("Unexpected character");
                    }
                }
                advance();
                // Move to next after '}'
                start = current;
            } else {
                // Process identifier
                start = current;
                processIdentifier(stringTokens);
                start = current;
            }
        } else if (c == '\\') {
            // cap
            if (match('$')) {
                start = current - 1;
            }
        }
    }
    if (isAtEnd()) {
        error("Unterminated string literal");
        return;
    }
    // Peek is '"', move to next
    advance();
    // We don't need '"' in string, so minus 1
    addToken(stringTokens, STRING, start, current - start - 1);
    stringTokens.push_back(new Token(FILE_EOF, "", line));
    auto *token = new StringToken(codes->substr(originalStart + 1, current - originalStart - 2), line,
                                  stringTokens);
    tokens.push_back(token);
}

void Lexer::error(const std::string &error) const {
    Logger::instance()->logError(line, error);
}

void Lexer::printTokens() const {
    for (const auto &token: tokens) {
        std::cout << token->toString() << std::endl;
    }
}

ulong Lexer::tokenSize() const {
    return tokens.size();
}

Token *Lexer::getTokenAt(const ulong index) const {
    return tokens.at(index);
}

const std::vector<Token *> *Lexer::getTokens() const {
    return &tokens;
}
