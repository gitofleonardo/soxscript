//
// Created by hhvvg on 8/18/24.
//

#ifndef UTILS_HPP
#define UTILS_HPP
#include <map>
#include <memory>

#include "../interpret/callable.hpp"
#include "../lexical/token_type.hpp"

class SoxKeywords {
    std::map<std::string, TokenType> _keywords;
    static SoxKeywords *_sInstance;

    SoxKeywords() {
        _keywords.insert(std::make_pair("else", ELSE));
        _keywords.insert(std::make_pair("false", FALSE));
        _keywords.insert(std::make_pair("for", FOR));
        _keywords.insert(std::make_pair("fun", FUN));
        _keywords.insert(std::make_pair("if", IF));
        _keywords.insert(std::make_pair("null", NULL_PTR));
        _keywords.insert(std::make_pair("return", RETURN));
        _keywords.insert(std::make_pair("true", TRUE));
        _keywords.insert(std::make_pair("var", VAR));
        _keywords.insert(std::make_pair("while", WHILE));
        _keywords.insert(std::make_pair("varargs", VARARGS));
    }

public:
    ~SoxKeywords() {
        delete _sInstance;
    }

    [[nodiscard]] TokenType getKeyword(const std::string &kw) const {
        if (!_keywords.contains(kw)) {
            return IDENTIFIER;
        }
        return _keywords.at(kw);
    }

    static SoxKeywords *instance() {
        return _sInstance;
    }
};

inline bool isLetter(const char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline bool isDigit(const char c) {
    return c >= '0' && c <= '9';
}

inline bool isLegalIdentifierPrefix(const char c) {
    return c == '_' || isLetter(c);
}

inline bool isLegalIdentifierChar(const char c) {
    return isLegalIdentifierPrefix(c) || isDigit(c);
}

inline bool isLegalHexNumber(const char c) {
    return c >= '0' && c <= '9' || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

inline bool isLegalOctalNumber(const char c) {
    return c >= '0' && c <= '7';
}

inline bool isLegalBinaryNumber(const char c) {
    return c >= '0' && c <= '1';
}

inline std::shared_ptr<Callable> makeSharedCallable(Callable *callable) {
    return std::shared_ptr<Callable>(callable);
}

#endif //UTILS_HPP
