//
// Created by hhvvg on 8/20/24.
//

#ifndef RESOLVER_HPP
#define RESOLVER_HPP
#include <map>
#include <stack>
#include <string>

#include "interpreter.hpp"
#include "../parser/expr.hpp"
#include "../parser/stmt.hpp"

enum BlockType {
    GLOBAL, FUNCTION
};

class Resolver final : public ExprVisitor<void>, public StmtVisitor<void> {
    std::vector<std::map<std::string, bool>> _scopes;
    BlockType _block_type = GLOBAL;

    Interpreter *_interpreter;

    void resolve(Expr *expr);

    void resolve(Stmt *stmt);

    void resolveFunction(const FunctionStmt *func);

    void resolveLocalVariable(Expr *expr, const std::string *name) const;

    void beginScope();

    void endScope();

    void define(const Token *name);

    void declare(const Token *name) const;

protected:
    void visitBinaryExpr(BinaryExpr *expr) override;

    void visitGroupingExpr(GroupingExpr *expr) override;

    void visitLiteralExpr(LiteralExpr *expr) override;

    void visitUnaryExpr(UnaryExpr *expr) override;

    void visitTernaryExpr(TernaryExpr *expr) override;

    void visitVariableExpr(VariableExpr *expr) override;

    void visitAssignExpr(AssignExpr *expr) override;

    void visitLogicalExpr(LogicalExpr *expr) override;

    void visitCallExpr(CallExpr *expr) override;

    void visitArrayExpr(ArrayExpr *expr) override;

    void visitIndexedCallExpr(IndexedCallExpr *expr) override;

    void visitExprStmt(ExprStmt *stmt) override;

    void visitVarStmt(VarStmt *stmt) override;

    void visitBlockStmt(BlockStmt *stmt) override;

    void visitIfStmt(IfStmt *stmt) override;

    void visitWhileStmt(WhileStmt *stmt) override;

    void visitFunctionStmt(FunctionStmt *stmt) override;

    void visitReturnStmt(ReturnStmt *stmt) override;

    void visitIndexedEleAssignExpr(ArrayElementAssignExpr *expr) override;

    void visitMapExpr(MapExpr *expr) override;

public:
    explicit Resolver(Interpreter *interpreter);
    ~Resolver() override;

    void resolve(std::vector<Stmt *> *stmts);
};

#endif //RESOLVER_HPP
