//
// Created by hhvvg on 8/18/24.
//

#ifndef EXPR_HPP
#define EXPR_HPP
#include <stdexcept>
#include <vector>

#include "../lexical/token.hpp"

template<class R>
class ExprVisitor;

class Expr {
public:
    template<class R>
    R accept(ExprVisitor<R> *visitor) {
        return visitor->visitExpr(this);
    }

    virtual ~Expr() = default;
};

class BinaryExpr final : public Expr {
public:
    Expr *left;
    Expr *right;
    Token *op;

    BinaryExpr(Expr *left, Token *op, Expr *right): left(left), right(right), op(op) {
    }

    ~BinaryExpr() override {
        delete left;
        delete right;
    }
};

class GroupingExpr final : public Expr {
public:
    Expr *expr;

    explicit GroupingExpr(Expr *expr): expr(expr) {
    }

    ~GroupingExpr() override {
        delete expr;
    }
};

class UnaryExpr final : public Expr {
public:
    Expr *right;
    Token *op;

    UnaryExpr(Expr *right, Token *op): right(right), op(op) {
    }

    ~UnaryExpr() override {
        delete right;
    }
};

class LiteralExpr final : public Expr {
public:
    const Token *value;

    explicit LiteralExpr(const Token *value): value(value) {
    }

    ~LiteralExpr() override = default;
};

class TernaryExpr final : public Expr {
public:
    Expr *left;
    Expr *right;
    Expr *condition;

    TernaryExpr(Expr *left, Expr *right, Expr *condition): left(left), right(right),
                                                           condition(condition) {
    }

    ~TernaryExpr() override {
        delete left;
        delete right;
        delete condition;
    }
};

class VariableExpr final : public Expr {
public:
    const Token *name;

    explicit VariableExpr(const Token *name): name(name) {
    }

    ~VariableExpr() override = default;
};

class AssignExpr final : public Expr {
public:
    Expr *value;
    const Token *name;

    explicit AssignExpr(Expr *value, const Token *name): value(value), name(name) {
    }

    ~AssignExpr() override {
        delete value;
    }
};

class LogicalExpr final : public Expr {
public:
    Expr *left;
    Expr *right;
    Token *op;

    LogicalExpr(Expr *left, Expr *right, Token *op): left(left), right(right), op(op) {
    }

    ~LogicalExpr() override {
        delete left;
        delete right;
    }
};

class CallExpr final : public Expr {
public:
    Expr *callee;
    const Token *paren;
    const std::vector<Expr *> *arguments;

    CallExpr(Expr *callee, const Token *paren, const std::vector<Expr *> *arguments): callee(callee),
        paren(paren),
        arguments(arguments) {
    }

    ~CallExpr() override {
        delete callee;
        for (auto &arg: *arguments) {
            delete arg;
        }
        delete arguments;
    }
};

class IndexedCallExpr final : public Expr {
public:
    Expr *callee;
    const Token *bracket;
    Expr *index;

    IndexedCallExpr(Expr *callee, const Token *bracket, Expr *index): callee(callee), bracket(bracket),
                                                                      index(index) {
    }

    ~IndexedCallExpr() override {
        delete callee;
        delete index;
    }
};

class ArrayExpr final : public Expr {
public:
    Token *bracket;
    std::vector<Expr *> *elements;

    ArrayExpr(Token *bracket, std::vector<Expr *> *elements): bracket(bracket), elements(elements) {
    }

    ~ArrayExpr() override {
        for (const auto &element: *elements) {
            delete element;
        }
        delete elements;
    }
};

class ArrayElementAssignExpr final : public Expr {
public:
    Expr *callee;
    Expr *index;
    Expr *value;
    const Token *bracket;

    ArrayElementAssignExpr(Expr *callee, Expr *index, Expr *value, const Token *bracket): callee(callee), index(index),
        value(value), bracket(bracket) {
    }

    ~ArrayElementAssignExpr() override {
        delete callee;
        delete index;
    }
};

template<class R>
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;

    R visitExpr(Expr *expr) {
        if (const auto b = dynamic_cast<BinaryExpr *>(expr)) {
            return visitBinaryExpr(b);
        }
        if (const auto c = dynamic_cast<CallExpr *>(expr)) {
            return visitCallExpr(c);
        }
        if (const auto g = dynamic_cast<GroupingExpr *>(expr)) {
            return visitGroupingExpr(g);
        }
        if (const auto l = dynamic_cast<LiteralExpr *>(expr)) {
            return visitLiteralExpr(l);
        }
        if (const auto u = dynamic_cast<UnaryExpr *>(expr)) {
            return visitUnaryExpr(u);
        }
        if (const auto tn = dynamic_cast<TernaryExpr *>(expr)) {
            return visitTernaryExpr(tn);
        }
        if (const auto v = dynamic_cast<VariableExpr *>(expr)) {
            return visitVariableExpr(v);
        }
        if (const auto a = dynamic_cast<AssignExpr *>(expr)) {
            return visitAssignExpr(a);
        }
        if (const auto lg = dynamic_cast<LogicalExpr *>(expr)) {
            return visitLogicalExpr(lg);
        }
        if (const auto arr = dynamic_cast<ArrayExpr *>(expr)) {
            return visitArrayExpr(arr);
        }
        if (const auto index = dynamic_cast<IndexedCallExpr *>(expr)) {
            return visitIndexedCallExpr(index);
        }
        if (const auto aeae = dynamic_cast<ArrayElementAssignExpr *>(expr)) {
            return visitArrayEleAssignExpr(aeae);
        }
        // This should not happen.
        throw std::runtime_error("Unknown expr type");
    }

protected:
    virtual R visitBinaryExpr(BinaryExpr *expr) = 0;

    virtual R visitGroupingExpr(GroupingExpr *expr) = 0;

    virtual R visitLiteralExpr(LiteralExpr *expr) = 0;

    virtual R visitUnaryExpr(UnaryExpr *expr) = 0;

    virtual R visitTernaryExpr(TernaryExpr *expr) = 0;

    virtual R visitVariableExpr(VariableExpr *expr) = 0;

    virtual R visitAssignExpr(AssignExpr *expr) = 0;

    virtual R visitLogicalExpr(LogicalExpr *expr) = 0;

    virtual R visitCallExpr(CallExpr *expr) = 0;

    virtual R visitArrayExpr(ArrayExpr *expr) = 0;

    virtual R visitIndexedCallExpr(IndexedCallExpr *expr) = 0;

    virtual R visitArrayEleAssignExpr(ArrayElementAssignExpr *expr) = 0;
};

#endif //EXPR_HPP
