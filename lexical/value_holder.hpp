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

    virtual bool equals(const ValueHolder *other) {
        return false;
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

    bool equals(const ValueHolder *other) override {
        if (const auto otherValue = dynamic_cast<const StringValueHolder *>(other)) {
            return value == otherValue->value;
        }
        return false;
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

    bool equals(const ValueHolder *other) override {
        if (const auto otherValue = dynamic_cast<const IntegerValueHolder *>(other)) {
            return value == otherValue->value;
        }
        return false;
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

    bool equals(const ValueHolder *other) override {
        if (const auto otherValue = dynamic_cast<const DoubleValueHolder *>(other)) {
            return value == otherValue->value;
        }
        return false;
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

    bool equals(const ValueHolder *other) override {
        if (const auto otherValue = dynamic_cast<const BoolValueHolder *>(other)) {
            return value == otherValue->value;
        }
        return false;
    }
};

class Callable;

class CallableHolder final : public ValueHolder {
public:
    std::vector<std::shared_ptr<Callable> > callables;

    explicit CallableHolder(const std::shared_ptr<Callable> &callable) {
        callables.push_back(callable);
    }

    bool equals(const ValueHolder *other) override {
        if (const auto otherValue = dynamic_cast<const CallableHolder *>(other)) {
            if (otherValue->callables.size() != callables.size()) {
                return false;
            }
            for (auto i = 0; i < callables.size(); ++i) {
                if (callables[i] != otherValue->callables[i]) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
};

class ArrayValueHolder final : public ValueHolder {
public:
    std::vector<std::shared_ptr<ValueHolder> > values;

    ArrayValueHolder() = default;

    bool equals(const ValueHolder *other) override {
        if (const auto otherValue = dynamic_cast<const ArrayValueHolder *>(other)) {
            if (values.size() != otherValue->values.size()) {
                return false;
            }
            for (auto i = 0; i < values.size(); ++i) {
                const auto holder = values[i];
                const auto otherHolder = otherValue->values[i];
                if (!(holder->equals(otherHolder.get()))) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
};

#endif //VALUE_HOLDER_HPP
