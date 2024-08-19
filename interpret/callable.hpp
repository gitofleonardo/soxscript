//
// Created by hhvvg on 8/20/24.
//

#ifndef CALLABLE_HPP
#define CALLABLE_HPP
#include <memory>
#include <utility>

#include "interpreter.hpp"
#include "runtime_scope.hpp"

class Token;
class FunctionStmt;
class ValueHolder;

class Callable {
public:
    virtual ~Callable() = default;

    virtual std::shared_ptr<ValueHolder> call(Interpreter *interpreter,
                                              const std::vector<std::shared_ptr<ValueHolder> > &args) = 0;

    virtual int parameterSize() = 0;
};

// Returns a value from function
class FunctionCallable final : public Callable {
    FunctionStmt *_fun;
    std::shared_ptr<RuntimeScope> _scope;
    bool _isInitializer;

public:
    FunctionCallable(FunctionStmt *fun, std::shared_ptr<RuntimeScope> scope, const bool isInitializer): _fun(fun),
        _scope(std::move(scope)),
        _isInitializer(isInitializer) {
    }

    std::shared_ptr<ValueHolder>
    call(Interpreter *interpreter, const std::vector<std::shared_ptr<ValueHolder> > &args) override {
        if (_isInitializer) {
            return _scope->get(0, "this");
        }
        const auto funScope = std::make_shared<RuntimeScope>(_scope);
        for (int i = 0; i < args.size(); i++) {
            funScope->define(*_fun->params->at(i)->lexeme(), args[i]);
            ++i;
        }
        try {
            interpreter->executeBlock(_fun->bodyBlock->stmts, funScope);
            return nullptr;
        } catch (ReturnValue &ret) {
            return ret.value;
        }
    }

    int parameterSize() override {
        return static_cast<int>(_fun->params->size());
    }
};

#endif //CALLABLE_HPP
