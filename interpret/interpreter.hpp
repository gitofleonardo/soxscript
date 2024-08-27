//
// Created by hhvvg on 8/20/24.
//

#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP
#include <vector>

#include "runtime_scope.hpp"
#include "../parser/stmt.hpp"

class Interpreter final : public StmtVisitor<void>, public ExprVisitor<std::shared_ptr<ValueHolder> > {

    std::shared_ptr<RuntimeScope> _currentScope;
    std::shared_ptr<RuntimeScope> _globalScope;
    std::map<Expr *, int> _scopesTable;

    void execute(Stmt *stmt) const;

    static bool checkNumberOperand(Token *op, std::initializer_list<std::shared_ptr<ValueHolder> > operands);

    static bool isTruthy(const std::shared_ptr<ValueHolder>& value);

    static bool isDouble(const std::shared_ptr<ValueHolder>& value);

    static double asDouble(const std::shared_ptr<ValueHolder>& value);

    static int asInt(const std::shared_ptr<ValueHolder>& value);

    static bool isString(const std::shared_ptr<ValueHolder>& value);

    static std::string asString(const std::shared_ptr<ValueHolder>& value);

    std::shared_ptr<ValueHolder> lookup(const Token *name, Expr *expr);

public:
    Interpreter();

    ~Interpreter() override;

    void interpret(std::vector<Stmt *> *stmts) const;

    void resolve(int depth, Expr *expr);

protected:
    void visitExprStmt(ExprStmt *stmt) override;

    void visitVarStmt(VarStmt *stmt) override;

    void visitBlockStmt(BlockStmt *stmt) override;

    void visitIfStmt(IfStmt *stmt) override;

    void visitWhileStmt(WhileStmt *stmt) override;

    void visitFunctionStmt(FunctionStmt *stmt) override;

    void visitReturnStmt(ReturnStmt *stmt) override;

    std::shared_ptr<ValueHolder> visitBinaryExpr(BinaryExpr *expr) override;

    std::shared_ptr<ValueHolder> visitGroupingExpr(GroupingExpr *expr) override;

    std::shared_ptr<ValueHolder> visitLiteralExpr(LiteralExpr *expr) override;

    std::shared_ptr<ValueHolder> visitUnaryExpr(UnaryExpr *expr) override;

    std::shared_ptr<ValueHolder> visitTernaryExpr(TernaryExpr *expr) override;

    std::shared_ptr<ValueHolder> visitVariableExpr(VariableExpr *expr) override;

    std::shared_ptr<ValueHolder> visitAssignExpr(AssignExpr *expr) override;

    std::shared_ptr<ValueHolder> visitLogicalExpr(LogicalExpr *expr) override;

    std::shared_ptr<ValueHolder> visitCallExpr(CallExpr *expr) override;

    std::shared_ptr<ValueHolder> visitArrayExpr(ArrayExpr *expr) override;

    std::shared_ptr<ValueHolder> visitIndexedCallExpr(IndexedCallExpr *expr) override;

    std::shared_ptr<ValueHolder> visitIndexedEleAssignExpr(ArrayElementAssignExpr *expr) override;

    std::shared_ptr<ValueHolder> visitMapExpr(MapExpr *expr) override;

    std::shared_ptr<ValueHolder> visitPrefixAutoUnaryExpr(PrefixAutoUnaryExpr *expr) override;

    std::shared_ptr<ValueHolder> visitSuffixAutoUnaryExpr(SuffixAutoUnaryExpr *expr) override;
public:
    void executeBlock(std::vector<Stmt *> *stmts, std::shared_ptr<RuntimeScope> scope);
    std::shared_ptr<ValueHolder> evaluate(Expr *expr) const;
};

#endif //INTERPRETER_HPP
