//
// Created by hhvvg on 8/20/24.
//

#include "resolver.hpp"

#include "../utils/logger.hpp"

Resolver::Resolver(Interpreter *interpreter): _interpreter(interpreter) {
}

void Resolver::visitBinaryExpr(BinaryExpr *expr) {
    resolve(expr->left);
    resolve(expr->right);
}

void Resolver::visitGroupingExpr(GroupingExpr *expr) {
    resolve(expr->expr);
}

void Resolver::visitLiteralExpr(LiteralExpr *expr) {
}

void Resolver::visitUnaryExpr(UnaryExpr *expr) {
    resolve(expr->right);
}

void Resolver::visitTernaryExpr(TernaryExpr *expr) {
    resolve(expr->condition);
    resolve(expr->left);
    resolve(expr->right);
}

void Resolver::visitVariableExpr(VariableExpr *expr) {
    if (!_scopes.empty() && _scopes.back().contains(expr->name->lexeme()) && _scopes.back()[expr->name->lexeme()] ==
        false) {
        Logger::instance()->logError(expr->name, "");
    }
    resolveLocalVariable(expr, expr->name->lexeme());
}

void Resolver::visitAssignExpr(AssignExpr *expr) {
    resolve(expr->value);
    resolveLocalVariable(expr, expr->name->lexeme());
}

void Resolver::visitLogicalExpr(LogicalExpr *expr) {
    resolve(expr->left);
    resolve(expr->right);
}

void Resolver::visitCallExpr(CallExpr *expr) {
    resolve(expr->callee);
    for (const auto arg: *expr->arguments) {
        resolve(arg);
    }
}

Resolver::~Resolver() = default;

void Resolver::visitExprStmt(ExprStmt *stmt) {
    resolve(stmt->expr);
}

void Resolver::visitVarStmt(VarStmt *stmt) {
    declare(stmt->name);
    if (stmt->initializer) {
        resolve(stmt->initializer);
    }
    define(stmt->name);
}

void Resolver::visitBlockStmt(BlockStmt *stmt) {
    beginScope();
    resolve(stmt->stmts);
    endScope();
}

void Resolver::visitIfStmt(IfStmt *stmt) {
    resolve(stmt->condition);
    resolve(stmt->thenBlock);
    if (stmt->elseBlock) {
        resolve(stmt->elseBlock);
    }
}

void Resolver::visitWhileStmt(WhileStmt *stmt) {
    resolve(stmt->condition);
    resolve(stmt->body);
}

void Resolver::visitFunctionStmt(FunctionStmt *stmt) {
    declare(stmt->name);
    define(stmt->name);
    resolveFunction(stmt);
}

void Resolver::visitReturnStmt(ReturnStmt *stmt) {
    if (_block_type != FUNCTION) {
        throw RuntimeError(stmt->keyword, "Cannot return from outside a function");
    }
    resolve(stmt->value);
}

void Resolver::resolve(Expr *expr) {
    expr->accept((ExprVisitor *) this);
}

void Resolver::resolve(Stmt *stmt) {
    stmt->accept((StmtVisitor *) this);
}

void Resolver::resolve(std::vector<Stmt *> *stmts) {
    for (const auto stmt: *stmts) {
        resolve(stmt);
    }
}

void Resolver::resolveFunction(const FunctionStmt *func) {
    const auto enclosingType = _block_type;
    _block_type = FUNCTION;
    beginScope();
    for (const auto param: *func->params) {
        declare(param->name);
        define(param->name);
    }
    resolve(func->bodyBlock->stmts);
    endScope();
    _block_type = enclosingType;
}

void Resolver::resolveLocalVariable(Expr *expr, const std::string &name) const {
    for (int i = static_cast<int>(_scopes.size()) - 1; i >= 0; i--) {
        if (_scopes[i].contains(name)) {
            _interpreter->resolve(static_cast<int>(_scopes.size()) - 1 - i, expr);
            return;
        }
    }
}

void Resolver::beginScope() {
    _scopes.emplace_back();
}

void Resolver::endScope() {
    _scopes.pop_back();
}

void Resolver::define(const Token *name) {
    if (_scopes.empty()) {
        return;
    }
    _scopes.back()[name->lexeme()] = true;
}

void Resolver::declare(const Token *name) const {
    if (_scopes.empty()) {
        return;
    }
    auto top = _scopes.back();
    if (top.contains(name->lexeme())) {
        Logger::instance()->logError(name, "Variable already declared.");
    }
    top[name->lexeme()] = false;
}

void Resolver::visitArrayExpr(ArrayExpr *expr) {
    for (const auto element: *expr->elements) {
        resolve(element);
    }
}

void Resolver::visitIndexedCallExpr(IndexedCallExpr *expr) {
    resolve(expr->callee);
    resolve(expr->index);
}

void Resolver::visitIndexedEleAssignExpr(ArrayElementAssignExpr *expr) {
    resolve(expr->callee);
    resolve(expr->index);
}

void Resolver::visitMapExpr(MapExpr *expr) {
    for (const auto [fst, snd]: *expr->elements) {
        resolve(fst);
        resolve(snd);
    }
}

void Resolver::visitPrefixAutoUnaryExpr(PrefixAutoUnaryExpr *expr) {
    resolve(expr->expr);
}

void Resolver::visitSuffixAutoUnaryExpr(SuffixAutoUnaryExpr *expr) {
    resolve(expr->expr);
}

void Resolver::visitStringLiteralExpr(StringLiteralExpr *expr) {
    for (Expr *e : expr->values) {
        resolve(e);
    }
}
