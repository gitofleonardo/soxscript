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

    [[nodiscard]] bool isVarargsParams() const {
        if (_fun->params->empty()) {
            return false;
        }
        return _fun->params->at(_fun->params->size() - 1)->isVararg;
    }

public:
    const bool isVarargs;

    FunctionCallable(FunctionStmt *fun, std::shared_ptr<RuntimeScope> scope, const bool isInitializer): _fun(fun),
        _scope(std::move(scope)),
        _isInitializer(isInitializer), isVarargs(isVarargsParams()) {
    }

    std::shared_ptr<ValueHolder>
    call(Interpreter *interpreter, const std::vector<std::shared_ptr<ValueHolder> > &args) override {
        if (_isInitializer) {
            return _scope->get(0, "this");
        }
        const auto funScope = std::make_shared<RuntimeScope>(_scope);
        auto argsIndex = 0;
        for (const auto argsEnd = isVarargs ? _fun->params->size() - 1 : _fun->params->size(); argsIndex < argsEnd; ++
             argsIndex) {
            const auto param = _fun->params->at(argsIndex);
            funScope->define(*param->name->lexeme(), args[argsIndex]);
        }
        if (isVarargs) {
            const auto arrayParams = std::make_shared<ArrayValueHolder>();
            while (argsIndex < args.size()) {
                arrayParams->values.push_back(args[argsIndex]);
                ++argsIndex;
            }
            funScope->define(*_fun->params->at(_fun->params->size() - 1)->name->lexeme(), arrayParams);
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
