//
// Created by hhvvg on 8/18/24.
//

#ifndef VALUE_HOLDER_HPP
#define VALUE_HOLDER_HPP

#include <string>
#include <vector>

#include "token.hpp"

class ValueHolder {
public:
    virtual ~ValueHolder() = default;

    virtual std::string toString() {
        return "null";
    }
};

class StringValueHolder final : public ValueHolder {
public:
    explicit StringValueHolder(std::string &value) : value(std::move(value)) {
    }

    std::string value;

    std::string toString() override {
        return value;
    }
};

class IntegerValueHolder final : public ValueHolder {
public:
    explicit IntegerValueHolder(const int value) : value(value) {
    }

    int value;

    void invert() {
        value = -value;
    }

    std::string toString() override {
        return std::to_string(value);
    }
};

class DoubleValueHolder final : public ValueHolder {
public:
    explicit DoubleValueHolder(const double value) : value(value) {
    }

    double value;

    void invert() {
        value = -value;
    }

    std::string toString() override {
        return std::to_string(value);
    }
};

class BoolValueHolder final : public ValueHolder {
public:
    explicit BoolValueHolder(const bool value): value(value) {
    }

    bool value;

    void invert() {
        value = -value;
    }

    std::string toString() override {
        return value ? "true" : "false";
    }
};

class Callable;

class CallableHolder final : public ValueHolder {
public:
    std::vector<std::shared_ptr<Callable>> callables;

    explicit CallableHolder(const std::shared_ptr<Callable>& callable) {
        callables.push_back(callable);
    }
};

#endif //VALUE_HOLDER_HPP
