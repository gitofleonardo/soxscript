//
// Created by hhvvg on 8/18/24.
//

#ifndef STMT_HPP
#define STMT_HPP
#include "expr.hpp"
#include "../utils/exception.hpp"

template<class R>
class StmtVisitor;

class Stmt {
public:
    template<class R>
    R accept(StmtVisitor<R> *visitor) {
        return visitor->visitStmt(this);
    }

    virtual ~Stmt() = 0;
};

inline Stmt::~Stmt() = default;

class ExprStmt final : public Stmt {
public:
    Expr *expr;

    explicit ExprStmt(Expr *expr) : expr(expr) {
    }

    ~ExprStmt() override {
        delete expr;
    }
};

class VarStmt final : public Stmt {
public:
    Token *name;
    Expr *initializer;

    VarStmt(Token *name, Expr *initializer) : name(name), initializer(initializer) {
    }

    ~VarStmt() override {
        delete initializer;
    }
};

class BlockStmt final : public Stmt {
public:
    std::vector<Stmt *> *stmts;

    explicit BlockStmt(std::vector<Stmt *> *stmts) : stmts(stmts) {
    }

    ~BlockStmt() override {
        for (const auto stmt: *stmts) {
            delete stmt;
        }
        delete stmts;
    }
};

class IfStmt final : public Stmt {
public:
    Expr *condition;
    Stmt *thenBlock;
    Stmt *elseBlock;

    IfStmt(Expr *condition, Stmt *thenBlock, Stmt *elseBlock): condition(condition),
                                                               thenBlock(thenBlock),
                                                               elseBlock(elseBlock) {
    }

    ~IfStmt() override {
        delete condition;
        delete thenBlock;
        delete elseBlock;
    }
};

class WhileStmt final : public Stmt {
public:
    Expr *condition;
    Stmt *body;

    WhileStmt(Expr *condition, Stmt *body): condition(condition), body(body) {
    }

    ~WhileStmt() override {
        delete condition;
        delete body;
    }
};

class FunctionParam final {
public:
    Token *name;
    const bool isVararg = false;

    FunctionParam(Token *name, const bool isVararg) : name(name), isVararg(isVararg) {
    }
};

class FunctionStmt final : public Stmt {
public:
    Token *name;
    std::vector<FunctionParam *> *params;
    BlockStmt *bodyBlock;

    FunctionStmt(Token *name, std::vector<FunctionParam *> *params, BlockStmt *block): name(name), params(params),
        bodyBlock(block) {
    }

    ~FunctionStmt() override {
        delete params;
        delete bodyBlock;
    }
};

class ReturnStmt final : public Stmt {
public:
    Expr *value;
    Token *keyword;

    ReturnStmt(Expr *value, Token *keyword) : value(value), keyword(keyword) {
    }

    ~ReturnStmt() override {
        delete value;
    }
};

template<class R>
class StmtVisitor {
public:
    R visitStmt(Stmt *stmt) {
        if (const auto s = dynamic_cast<ExprStmt *>(stmt)) {
            return visitExprStmt(s);
        }
        if (const auto v = dynamic_cast<VarStmt *>(stmt)) {
            return visitVarStmt(v);
        }
        if (const auto b = dynamic_cast<BlockStmt *>(stmt)) {
            return visitBlockStmt(b);
        }
        if (const auto i = dynamic_cast<IfStmt *>(stmt)) {
            return visitIfStmt(i);
        }
        if (const auto w = dynamic_cast<WhileStmt *>(stmt)) {
            return visitWhileStmt(w);
        }
        if (const auto f = dynamic_cast<FunctionStmt *>(stmt)) {
            return visitFunctionStmt(f);
        }
        if (const auto r = dynamic_cast<ReturnStmt *>(stmt)) {
            return visitReturnStmt(r);
        }
        // This should not happen
        throw RuntimeError("This shouldn't happen");
    }

    virtual ~StmtVisitor() = default;

protected:
    virtual R visitExprStmt(ExprStmt *stmt) = 0;

    virtual R visitVarStmt(VarStmt *stmt) = 0;

    virtual R visitBlockStmt(BlockStmt *stmt) = 0;

    virtual R visitIfStmt(IfStmt *stmt) = 0;

    virtual R visitWhileStmt(WhileStmt *stmt) = 0;

    virtual R visitFunctionStmt(FunctionStmt *stmt) = 0;

    virtual R visitReturnStmt(ReturnStmt *stmt) = 0;
};

#endif //STMT_HPP
