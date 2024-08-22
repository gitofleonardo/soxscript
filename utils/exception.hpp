//
// Created by hhvvg on 8/19/24.
//

#ifndef ERROR_HPP
#define ERROR_HPP
#include <exception>
#include <utility>
#include <memory>

#include "../lexical/value_holder.hpp"

class ParserError final : public std::exception {
};

class RuntimeError final : public std::exception {
public:
    const Token *_token = nullptr;
    const std::string _message;

    RuntimeError(const Token *token, std::string message): _token(token), _message(std::move(message)) {
    }

    RuntimeError() = default;
};

class ReturnValue final : public std::exception {
public:
    std::shared_ptr<ValueHolder> value;

    explicit ReturnValue(std::shared_ptr<ValueHolder> value) : value(std::move(value)) {
    }
};

#endif //ERROR_HPP
