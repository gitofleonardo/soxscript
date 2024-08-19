//
// Created by hhvvg on 8/20/24.
//

#include "interpreter.hpp"

#include <memory>
#include <utility>

#include "builtin.hpp"
#include "callable.hpp"
#include "../utils/exception.hpp"
#include "../utils/logger.hpp"

void Interpreter::execute(Stmt *stmt) const {
    stmt->accept((StmtVisitor<void> *) this);
}

std::shared_ptr<ValueHolder> Interpreter::evaluate(Expr *expr) const {
    if (expr == nullptr) return nullptr;
    return expr->accept((ExprVisitor<std::shared_ptr<ValueHolder> > *) this);
}

bool Interpreter::checkNumberOperand(Token *op, const std::initializer_list<std::shared_ptr<ValueHolder> > operands) {
    for (const auto &operand: operands) {
        if (const auto d = dynamic_cast<DoubleValueHolder *>(operand.get()); d != nullptr) {
            continue;
        }
        if (const auto integer = dynamic_cast<IntegerValueHolder *>(operand.get()); integer != nullptr) {
            continue;
        }
        throw RuntimeError();
    }
    return true;
}

bool Interpreter::isTruthy(const std::shared_ptr<ValueHolder> &value) {
    if (value == nullptr) return false;
    if (const auto b = dynamic_cast<BoolValueHolder *>(value.get()); b != nullptr) {
        return b->value;
    }
    if (const auto integer = dynamic_cast<IntegerValueHolder *>(value.get()); integer != nullptr) {
        return integer->value != 0;
    }
    return true;
}

Interpreter::Interpreter() {
    _globalScope = std::make_shared<RuntimeScope>(std::shared_ptr<RuntimeScope>(nullptr));
    _currentScope = _globalScope;
    initGlobalScope(_globalScope.get());
}

Interpreter::~Interpreter() = default;

void Interpreter::interpret(std::vector<Stmt *> *stmts) const {
    for (const auto stmt: *stmts) {
        execute(stmt);
    }
}

void Interpreter::visitExprStmt(ExprStmt *stmt) {
    evaluate(stmt->expr);
}

void Interpreter::visitVarStmt(VarStmt *stmt) {
    const std::shared_ptr<ValueHolder> val = stmt->initializer != nullptr
                                                 ? evaluate(stmt->initializer)
                                                 : RuntimeScope::UNINITIALIZED_OBJECT;
    _currentScope->define(*stmt->name->lexeme(), val);
}

void Interpreter::visitBlockStmt(BlockStmt *stmt) {
    const auto blockScope = std::make_shared<RuntimeScope>(_currentScope);
    executeBlock(stmt->stmts, blockScope);
}

void Interpreter::visitIfStmt(IfStmt *stmt) {
    if (const auto conditionVal = evaluate(stmt->condition); isTruthy(conditionVal)) {
        execute(stmt->thenBlock);
    } else if (stmt->elseBlock != nullptr) {
        execute(stmt->elseBlock);
    }
}

void Interpreter::visitWhileStmt(WhileStmt *stmt) {
    auto conditionValue = evaluate(stmt->condition);
    while (isTruthy(conditionValue)) {
        execute(stmt->body);
        conditionValue = evaluate(stmt->condition);
    }
}

void Interpreter::visitFunctionStmt(FunctionStmt *stmt) {
    const auto func = std::static_pointer_cast<ValueHolder>(
        std::make_shared<FunctionHolder>(new FunctionCallable(stmt, _currentScope, false)));
    _currentScope->define(*stmt->name->lexeme(), func);
}

void Interpreter::visitReturnStmt(ReturnStmt *stmt) {
    throw ReturnValue(evaluate(stmt->value));
}

std::shared_ptr<ValueHolder> Interpreter::visitBinaryExpr(BinaryExpr *expr) {
    auto leftVal = evaluate(expr->left);
    auto rightVal = evaluate(expr->right);
    std::shared_ptr<ValueHolder> result;
    switch (expr->op->type()) {
        case PLUS: {
            if (isString(leftVal) || isString(rightVal)) {
                auto str = asString(leftVal) + asString(rightVal);
                result = std::make_shared<StringValueHolder>(str);
            } else {
                checkNumberOperand(expr->op, {leftVal, rightVal});
                if (isDouble(leftVal) || isDouble(rightVal)) {
                    result = std::make_shared<DoubleValueHolder>(asDouble(leftVal) + asDouble(rightVal));
                } else {
                    result = std::make_shared<IntegerValueHolder>(asInt(leftVal) + asInt(rightVal));
                }
            }
            break;
        }
        case MINUS: {
            checkNumberOperand(expr->op, {leftVal, rightVal});
            if (isDouble(leftVal) || isDouble(rightVal)) {
                result = std::make_shared<DoubleValueHolder>(asDouble(leftVal) - asDouble(rightVal));
            } else {
                result = std::make_shared<IntegerValueHolder>(asInt(leftVal) - asInt(rightVal));
            }
            break;
        }
        case SLASH: {
            checkNumberOperand(expr->op, {leftVal, rightVal});
            if (isDouble(leftVal) || isDouble(rightVal)) {
                result = std::make_shared<DoubleValueHolder>(asDouble(leftVal) / asDouble(rightVal));
            } else {
                result = std::make_shared<IntegerValueHolder>(asInt(leftVal) / asInt(rightVal));
            }
            break;
        }
        case STAR: {
            checkNumberOperand(expr->op, {leftVal, rightVal});
            if (isDouble(leftVal) || isDouble(rightVal)) {
                result = std::make_shared<DoubleValueHolder>(asDouble(leftVal) * asDouble(rightVal));
            } else {
                result = std::make_shared<IntegerValueHolder>(asInt(leftVal) * asInt(rightVal));
            }
            break;
        }
        case GREATER: {
            checkNumberOperand(expr->op, {leftVal, rightVal});
            result = std::make_shared<BoolValueHolder>(asDouble(leftVal) > asDouble(rightVal));
            break;
        }
        case GREATER_EQUAL: {
            checkNumberOperand(expr->op, {leftVal, rightVal});
            result = std::make_shared<BoolValueHolder>(asDouble(leftVal) >= asDouble(rightVal));
            break;
        }
        case LESS: {
            checkNumberOperand(expr->op, {leftVal, rightVal});
            result = std::make_shared<BoolValueHolder>(asDouble(leftVal) < asDouble(rightVal));
            break;
        }
        case LESS_EQUAL: {
            checkNumberOperand(expr->op, {leftVal, rightVal});
            result = std::make_shared<BoolValueHolder>(asDouble(leftVal) <= asDouble(rightVal));
            break;
        }
        case EQUAL_EQUAL: {
            checkNumberOperand(expr->op, {leftVal, rightVal});
            result = std::make_shared<BoolValueHolder>(asDouble(leftVal) == asDouble(rightVal));
            break;
        }
        case BANG_EQUAL: {
            checkNumberOperand(expr->op, {leftVal, rightVal});
            result = std::make_shared<BoolValueHolder>(asDouble(leftVal) != asDouble(rightVal));
            break;
        }
        default: {
            throw RuntimeError();
        }
    }
    return result;
}

bool Interpreter::isDouble(const std::shared_ptr<ValueHolder> &value) {
    if (const auto d = dynamic_cast<DoubleValueHolder *>(value.get()); d != nullptr) {
        return true;
    }
    return false;
}

double Interpreter::asDouble(const std::shared_ptr<ValueHolder> &value) {
    if (isDouble(value)) {
        return dynamic_cast<DoubleValueHolder *>(value.get())->value;
    }
    if (const auto integer = dynamic_cast<IntegerValueHolder *>(value.get()); integer != nullptr) {
        return integer->value;
    }
    throw RuntimeError();
}

int Interpreter::asInt(const std::shared_ptr<ValueHolder> &value) {
    return static_cast<int>(asDouble(value));
}

bool Interpreter::isString(const std::shared_ptr<ValueHolder> &value) {
    if (const auto string = dynamic_cast<StringValueHolder *>(value.get()); string != nullptr) {
        return true;
    }
    return false;
}

std::string Interpreter::asString(const std::shared_ptr<ValueHolder> &value) {
    if (const auto string = dynamic_cast<StringValueHolder *>(value.get()); string != nullptr) {
        return string->value;
    }
    if (const auto integer = dynamic_cast<IntegerValueHolder *>(value.get()); integer != nullptr) {
        return std::to_string(integer->value);
    }
    if (const auto doubleValue = dynamic_cast<DoubleValueHolder *>(value.get()); doubleValue != nullptr) {
        return std::to_string(doubleValue->value);
    }
    throw RuntimeError();
}

std::shared_ptr<ValueHolder> Interpreter::visitGroupingExpr(GroupingExpr *expr) {
    return evaluate(expr->expr);
}

std::shared_ptr<ValueHolder> Interpreter::visitLiteralExpr(LiteralExpr *expr) {
    std::shared_ptr<ValueHolder> result;
    switch (expr->value->type()) {
        case STRING: {
            auto str = std::string(*expr->value->lexeme());
            result = std::make_shared<StringValueHolder>(str);
            break;
        }
        case TRUE: {
            result = std::make_shared<BoolValueHolder>(true);
            break;
        }
        case FALSE: {
            result = std::make_shared<BoolValueHolder>(false);
            break;
        }
        case DOUBLE: {
            result = std::make_shared<DoubleValueHolder>(std::stod(*expr->value->lexeme()));
            break;
        }
        case INT: {
            result = std::make_shared<IntegerValueHolder>(std::stoi(*expr->value->lexeme()));
            break;
        }
        default: {
            throw RuntimeError();
        }
    }
    return result;
}

std::shared_ptr<ValueHolder> Interpreter::visitUnaryExpr(UnaryExpr *expr) {
    auto right = evaluate(expr->right);
    switch (expr->op->type()) {
        case PLUS: {
            checkNumberOperand(expr->op, {right});
            if (isDouble(right)) {
                return right;
            }
            break;
        }
        case MINUS: {
            checkNumberOperand(expr->op, {right});
            if (isDouble(right)) {
                dynamic_cast<DoubleValueHolder *>(right.get())->invert();
                return right;
            }
            break;
        }
        case BANG: {
            right = std::make_shared<BoolValueHolder>(!isTruthy(right));
            return right;
        }
        default: {
            throw RuntimeError();
        }
    }
    // Unreachable
    return nullptr;
}

std::shared_ptr<ValueHolder> Interpreter::visitTernaryExpr(TernaryExpr *expr) {
    const auto conditionVal = evaluate(expr->condition);
    const auto leftVal = evaluate(expr->left);
    const auto rightVal = evaluate(expr->right);
    std::shared_ptr<ValueHolder> result;
    if (isTruthy(conditionVal)) {
        result = leftVal;
    } else {
        result = rightVal;
    }
    return result;
}

std::shared_ptr<ValueHolder> Interpreter::visitVariableExpr(VariableExpr *expr) {
    return lookup(expr->name, expr);
}

std::shared_ptr<ValueHolder> Interpreter::visitAssignExpr(AssignExpr *expr) {
    const auto value = evaluate(expr->value);
    if (_scopesTable.contains(expr)) {
        const int depth = _scopesTable[expr];
        _currentScope->assign(depth, *expr->name->lexeme(), value);
    } else {
        _globalScope->assign(*expr->name->lexeme(), value);
    }
    return value;
}

std::shared_ptr<ValueHolder> Interpreter::lookup(const Token *name, Expr *expr) {
    if (_scopesTable.contains(expr)) {
        return _currentScope->get(_scopesTable[expr], *name->lexeme());
    }
    return _globalScope->get(*name->lexeme());
}


std::shared_ptr<ValueHolder> Interpreter::visitLogicalExpr(LogicalExpr *expr) {
    auto left = evaluate(expr->left);
    if (expr->op->type() == OR) {
        if (isTruthy(left)) {
            return left;
        }
    } else {
        if (!isTruthy(left)) {
            return left;
        }
    }
    return evaluate(expr->right);
}

std::shared_ptr<ValueHolder> Interpreter::visitCallExpr(CallExpr *expr) {
    const auto callee = evaluate(expr->callee);
    Callable *callable;
    if (const auto holder = dynamic_cast<CallableHolder *>(callee.get())) {
        callable = holder->callable;
    } else if (const auto funcHolder = dynamic_cast<FunctionHolder *>(callee.get())) {
        callable = funcHolder->func;
    } else {
        throw RuntimeError();
    }
    if (expr->arguments->size() != callable->parameterSize()) {
        throw RuntimeError();
    }
    std::vector<std::shared_ptr<ValueHolder> > realArgs;
    for (const auto argument: *expr->arguments) {
        const auto argVal = evaluate(argument);
        realArgs.push_back(argVal);
    }
    const auto result = callable->call(this, realArgs);
    return result;
}

void Interpreter::executeBlock(std::vector<Stmt *> *stmts, std::shared_ptr<RuntimeScope> scope) {
    const auto prevScope = _currentScope;
    try {
        _currentScope = std::move(scope);
        for (const auto stmt: *stmts) {
            execute(stmt);
        }
    } catch ([[maybe_unused]] const RuntimeError &err) {
        throw;
    }
    _currentScope = prevScope;
}

void Interpreter::resolve(const int depth, Expr *expr) {
    _scopesTable[expr] = depth;
}
