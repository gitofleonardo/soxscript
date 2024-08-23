//
// Created by hhvvg on 8/20/24.
//

#include "runtime_scope.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

#include "callable.hpp"

std::shared_ptr<ValueHolder> RuntimeScope::UNINITIALIZED_OBJECT = std::make_shared<ValueHolder>();

RuntimeScope::RuntimeScope(std::shared_ptr<RuntimeScope> parentScope): _parent(std::move(parentScope)) {
}

std::shared_ptr<ValueHolder> RuntimeScope::define(const std::string &name, std::shared_ptr<ValueHolder> value) {
    std::shared_ptr<ValueHolder> oldVal = _definitions[name];
    const auto oldFun = dynamic_cast<CallableHolder *>(oldVal.get());
    if (const auto newFun = dynamic_cast<CallableHolder *>(value.get()); oldFun && newFun) {
        // Merge functions
        auto oldFunMap = std::map<int, std::shared_ptr<Callable>>();
        for (const auto &oldF : oldFun->callables) {
            oldFunMap[oldF->parameterSize()] = oldF;
        }
        for (const auto &newF: newFun->callables) {
            oldFunMap[newF->parameterSize()] = newF;
        }
        oldFun->callables.clear();
        for (const auto &oldF : oldFunMap) {
            oldFun->callables.push_back(oldF.second);
        }
    } else {
        // Replace symbol
        _definitions[name] = std::move(value);
    }
    return oldVal;
}

std::shared_ptr<ValueHolder> RuntimeScope::get(const std::string &name) {
    std::shared_ptr<ValueHolder> value;
    if (_definitions.contains(name)) {
        value = _definitions[name];
    } else if (_parent != nullptr) {
        value = _parent->get(name);
    } else {
        throw std::runtime_error("No such variable");
    }
    if (value == UNINITIALIZED_OBJECT) {
        throw std::runtime_error("Uninitialized variable");
    }
    return value;
}

RuntimeScope *RuntimeScope::ancestorScope(const int depth, RuntimeScope *root) {
    for (int i = 0; i < depth; i++) {
        root = root->_parent.get();
    }
    return root;
}

std::shared_ptr<ValueHolder> RuntimeScope::get(const int depth, const std::string &name) {
    return ancestorScope(depth, this)->_definitions.at(name);
}

std::shared_ptr<ValueHolder> RuntimeScope::assign(const std::string &name, const std::shared_ptr<ValueHolder> &value) {
    if (_definitions.contains(name)) {
        const auto oldVal = _definitions[name];
        _definitions[name] = value;
        return oldVal;
    }
    if (_parent != nullptr) {
        return _parent->assign(name, value);
    }
    throw std::runtime_error("No such variable");
}

std::shared_ptr<ValueHolder> RuntimeScope::assign(const int depth, const std::string &name,
                                                  std::shared_ptr<ValueHolder> value) {
    RuntimeScope *ancestor = ancestorScope(depth, this);
    const auto oldVal = ancestor->_definitions[name];
    ancestor->_definitions[name] = std::move(value);
    return oldVal;
}

RuntimeScope::~RuntimeScope() = default;
