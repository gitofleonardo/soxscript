//
// Created by hhvvg on 8/20/24.
//

#ifndef BUILTIN_HPP
#define BUILTIN_HPP
#include <iostream>

#include "callable.hpp"
#include "runtime_scope.hpp"
#include "../utils/utils.hpp"

class PrintCallable final : public Callable {
public:
    PrintCallable() = default;

    std::shared_ptr<ValueHolder>
    call(Interpreter *interpreter, const std::vector<std::shared_ptr<ValueHolder> > &args) override {
        std::cout << args.at(0)->toString();
        return nullptr;
    }

    int parameterSize() override {
        return 1;
    }
};

class PrintlnCallable final : public Callable {
public:
    PrintlnCallable() = default;

    std::shared_ptr<ValueHolder>
    call(Interpreter *interpreter, const std::vector<std::shared_ptr<ValueHolder> > &args) override {
        std::cout << args.at(0)->toString() << std::endl;
        return nullptr;
    }

    int parameterSize() override {
        return 1;
    }
};

class ArrayLengthCallable final : public Callable {
public:
    ArrayLengthCallable() = default;

    std::shared_ptr<ValueHolder>
    call(Interpreter *interpreter, const std::vector<std::shared_ptr<ValueHolder> > &args) override {
        const auto &holder = args.at(0);
        const auto arrHolder = dynamic_cast<ArrayValueHolder *>(holder.get());
        if (!arrHolder) {
            throw RuntimeError("Not an array");
        }
        return std::make_shared<IntegerValueHolder>(arrHolder->values.size());
    }

    int parameterSize() override {
        return 1;
    }
};

inline void initGlobalScope(RuntimeScope *globalScope) {
    globalScope->define("print", std::make_shared<CallableHolder>(makeSharedCallable(new PrintCallable)));
    globalScope->define("println", std::make_shared<CallableHolder>(makeSharedCallable(new PrintlnCallable)));
    globalScope->define("length", std::make_shared<CallableHolder>(makeSharedCallable(new ArrayLengthCallable)));
}

#endif //BUILTIN_HPP
