//
// Created by hhvvg on 8/18/24.
//

#ifndef VALUE_HOLDER_HPP
#define VALUE_HOLDER_HPP

#include <string>
#include <vector>
#include <unordered_map>

class ValueHolder {
public:
    virtual ~ValueHolder() = default;

    virtual std::string toString() {
        return "null";
    }

    virtual bool equals(const ValueHolder *other) {
        return this == other;
    }

    virtual ulong hash() {
        return reinterpret_cast<ulong>(static_cast<void *>(this));
    }
};

class StringValueHolder final : public ValueHolder {
    ulong _hash = 0;

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

    ulong hash() override {
        ulong h = _hash;
        if (const auto len = value.size(); h == 0 && len > 0) {
            for (int i = 0; i < len; i++) {
                h = h * 31 + value[i];
            }
            _hash = h;
        }
        return h;
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

    ulong hash() override {
        return value;
    }
};

typedef union {
    uint64_t bits;
    double d;
} Double;

class DoubleValueHolder final : public ValueHolder {
    Double _d{};

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

    ulong hash() override {
        _d.d = value;
        return _d.bits ^ (_d.bits >> 32);
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

    ulong hash() override {
        return value ? 1231 : 1237;
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

struct HolderHash {
    size_t operator()(const std::shared_ptr<ValueHolder> &value) const {
        return value->hash();
    }
};

struct HolderEquals {
    bool operator()(const std::shared_ptr<ValueHolder> &lhs, const std::shared_ptr<ValueHolder> &rhs) const {
        return lhs->equals(rhs.get());
    }
};

typedef std::unordered_map<std::shared_ptr<ValueHolder>, std::shared_ptr<ValueHolder>, HolderHash, HolderEquals> HolderMap;

class MapValueHolder final : public ValueHolder {
public:
    HolderMap values;

    MapValueHolder() = default;

    bool equals(const ValueHolder *other) override {
        if (const auto otherHolder = dynamic_cast<const MapValueHolder *>(other)) {
            const auto thisValues = values;
            const auto otherValues = otherHolder->values;
            if (thisValues.size() != otherValues.size()) {
                return false;
            }
            for (const auto &[key, value]: thisValues) {
                if (otherValues.contains(key)) {
                    return false;
                }
                if (const auto &otherValue = otherValues.at(key); !otherValue->equals(value.get())) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
};

#endif //VALUE_HOLDER_HPP
