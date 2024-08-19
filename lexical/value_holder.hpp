//
// Created by hhvvg on 8/18/24.
//

#ifndef VALUE_HOLDER_HPP
#define VALUE_HOLDER_HPP

#include <string>

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

class FunctionCallable;
class Callable;

class FunctionHolder final : public ValueHolder {
public:
    FunctionCallable *func;

    explicit FunctionHolder(FunctionCallable *function) : func(function) {
    }
};

class CallableHolder final : public ValueHolder {
public:
    Callable *callable;

    explicit CallableHolder(Callable *callable) : callable(callable) {
    }
};

#endif //VALUE_HOLDER_HPP
