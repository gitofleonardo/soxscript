//
// Created by hhvvg on 8/18/24.
//

#ifndef LOG_HPP
#define LOG_HPP
#include <iostream>
#include <ostream>
#include <string>

#include "../lexical/token.hpp"

class Logger {
    static Logger *sInstance;

    std::string AT_STR = "at ";
    bool _hasError = false;
    bool _hasRuntimeError = false;

    Logger() = default;

    void report(const uint line, const std::string &where, const std::string &message) {
        _hasError = true;
        std::cout << "[" << line << "] " << where << ": " << message << std::endl;
    }

public:
    static Logger* instance() {
        return sInstance;
    }

    void logError(const uint line, const std::string &error) {
        report(line, "", error);
    }

    void logError(const Token *token, const std::string &message) {
        if (token->type() == FILE_EOF) {
            report(token->line(), " at end", message);
        } else {
            report(token->line(), AT_STR + *token->lexeme(), message);
        }
    }

    void logRuntimeError(const uint line, const std::string &error) {
        _hasRuntimeError = true;
        report(line, "", error);
    }

    void reset() {
        _hasError = false;
        _hasRuntimeError = false;
    }

    [[nodiscard]] bool hasError() const {
        return _hasError;
    }

    [[nodiscard]] bool hasRuntimeError() const {
        return _hasRuntimeError;
    }

    ~Logger() {
        delete sInstance;
    }
};

#endif //LOG_HPP
