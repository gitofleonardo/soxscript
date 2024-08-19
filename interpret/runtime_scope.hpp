//
// Created by hhvvg on 8/20/24.
//

#ifndef SCOPE_HPP
#define SCOPE_HPP
#include <map>
#include <memory>
#include <string>

#include "../lexical/value_holder.hpp"

class RuntimeScope {
    std::shared_ptr<RuntimeScope> _parent;

    std::map<std::string, std::shared_ptr<ValueHolder> > _definitions;

    static RuntimeScope *ancestorScope(int depth, RuntimeScope *root);

public:
    static std::shared_ptr<ValueHolder> UNINITIALIZED_OBJECT;

    explicit RuntimeScope(std::shared_ptr<RuntimeScope> parentScope);

    std::shared_ptr<ValueHolder> define(const std::string &name, std::shared_ptr<ValueHolder> value);

    std::shared_ptr<ValueHolder> get(const std::string &name);

    std::shared_ptr<ValueHolder> get(int depth, const std::string &name);

    std::shared_ptr<ValueHolder> assign(const std::string &name, const std::shared_ptr<ValueHolder>& value);

    std::shared_ptr<ValueHolder> assign(int depth, const std::string &name, std::shared_ptr<ValueHolder> value);

    ~RuntimeScope();
};

#endif //SCOPE_HPP
