//
// Created by hhvvg on 8/18/24.
//

#include "parser.hpp"

#include "../utils/exception.hpp"
#include "../utils/logger.hpp"

Parser::Parser(const Lexer *lexer): lexer(lexer) {
}

Parser::~Parser() = default;

bool Parser::isAtEnd() const {
    return peek()->type() == FILE_EOF;
}

std::vector<Stmt *> *Parser::parse() {
    auto *stmts = new std::vector<Stmt *>();
    try {
        while (!isAtEnd()) {
            stmts->push_back(statement());
        }
    } catch ([[maybe_unused]] const std::exception &e) {
        synchronize();
    }
    return stmts;
}

Stmt *Parser::statement() {
    if (match(L_BRACE)) {
        return blockStatement();
    }
    if (match(IF)) {
        return ifStatement();
    }
    if (match(WHILE)) {
        return whileStatement();
    }
    if (match(FOR)) {
        return forStatement();
    }
    if (match(RETURN)) {
        return returnStatement();
    }
    return declarationStatement();
}

bool Parser::match(const std::initializer_list<TokenType> types) {
    for (auto &t: types) {
        if (check(t)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::match(const TokenType expected) {
    if (check(expected)) {
        advance();
        return true;
    }
    return false;
}


Stmt *Parser::returnStatement() {
    const auto keyword = previous();
    const auto value = check(SEMICOLON) ? nullptr : expression();
    consume(SEMICOLON, "Expected ';' after return statement");
    return new ReturnStmt(value, keyword);
}

Stmt *Parser::forStatement() {
    consume(L_PAREN, "Expected '(' after for statement");
    Stmt *initializer;
    if (match(SEMICOLON)) {
        initializer = nullptr;
    } else if (match(VAR)) {
        initializer = variableDeclarationStatement();
    } else {
        initializer = expressionStatement();
    }
    Expr *condition = !check(SEMICOLON) ? expression() : nullptr;
    consume(SEMICOLON, "Expected ';' after for statement");
    const auto increment = !check(SEMICOLON) ? expression() : nullptr;
    consume(R_PAREN, "Expected ')' after for statement");
    auto body = statement();
    if (increment) {
        auto *stmts = new std::vector<Stmt *>();
        stmts->push_back(body);
        stmts->push_back(new ExprStmt(increment));
        body = new BlockStmt(stmts);
    }
    body = new WhileStmt(condition, body);
    if (initializer) {
        auto *stmts = new std::vector<Stmt *>();
        stmts->push_back(initializer);
        stmts->push_back(body);
        body = new BlockStmt(stmts);
    }
    return body;
}

Stmt *Parser::whileStatement() {
    consume(L_PAREN, "Expected '(' after while statement");
    const auto condition = expression();
    consume(R_PAREN, "Expected ')' after while statement");
    const auto body = statement();
    return new WhileStmt(condition, body);
}

Stmt *Parser::ifStatement() {
    consume(L_PAREN, "Expected '(' after if statement");
    const auto condition = expression();
    consume(R_PAREN, "Expected ')' after if statement");
    const auto thenBranch = statement();
    const auto elseBranch = match(ELSE) ? statement() : nullptr;
    return new IfStmt(condition, thenBranch, elseBranch);
}

Stmt *Parser::declarationStatement() {
    if (match(VAR)) {
        return variableDeclarationStatement();
    }
    if (match(FUN)) {
        return functionDeclarationStatement();
    }
    return expressionStatement();
}

Stmt *Parser::functionDeclarationStatement() {
    const auto name = consume(IDENTIFIER, "Expected a function name");
    consume(L_PAREN, "Expect '(' after identifier");
    const auto params = new std::vector<Token *>();
    if (!check(R_PAREN)) {
        do {
            params->push_back(consume(IDENTIFIER, "Expect a parameter name"));
        } while (match(COMMA));
    }
    consume(R_PAREN, "Expect ')' after parameters");
    consume(L_BRACE, "Expect '{' on function declaration");
    const auto block = dynamic_cast<BlockStmt *>(blockStatement());
    if (block == nullptr) {
        error(peek(), "Missing function body");
    }
    return new FunctionStmt(name, params, block);
}

Stmt *Parser::variableDeclarationStatement() {
    const auto name = consume(IDENTIFIER, "Expected a variable name");
    const auto initializer = match(EQUAL) ? expression() : nullptr;
    consume(SEMICOLON, "Expected ';' after variable declaration");
    return new VarStmt(name, initializer);
}

Stmt *Parser::expressionStatement() {
    const auto expr = expression();
    consume(SEMICOLON, "Expected ';' after expression");
    return new ExprStmt(expr);
}

Stmt *Parser::blockStatement() {
    const auto stmts = new std::vector<Stmt *>();
    while (!check(R_BRACE) && !isAtEnd()) {
        stmts->push_back(statement());
    }
    consume(R_BRACE, "Expected '}' after block statement");
    return new BlockStmt(stmts);
}

Expr *Parser::expression() {
    return assignment();
}

Expr *Parser::assignment() {
    const auto expr = orExpression();

    if (match(EQUAL)) {
        const auto equals = previous();
        const auto rvalue = assignment();
        if (const auto v = dynamic_cast<VariableExpr *>(expr)) {
            const auto name = v->name;
            return new AssignExpr(rvalue, name);
        }
        if (const auto indexedCall = dynamic_cast<IndexedCallExpr *>(expr)) {
            return new ArrayElementAssignExpr(indexedCall->callee, indexedCall->index, rvalue, indexedCall->bracket);
        }
        error(equals, "Invalid assign rvalue");
    }
    return expr;
}

Expr *Parser::orExpression() {
    auto expr = andExpression();
    while (match(OR)) {
        const auto op = previous();
        const auto rvalue = andExpression();
        expr = new LogicalExpr(expr, rvalue, op);
    }
    return expr;
}

Expr *Parser::andExpression() {
    auto expr = ternaryExpression();
    while (match(AND)) {
        const auto op = previous();
        const auto rvalue = ternaryExpression();
        expr = new LogicalExpr(expr, rvalue, op);
    }
    return expr;
}

Expr *Parser::ternaryExpression() {
    auto expr = equalityExpression();
    while (match(QUESTION_MARK)) {
        const auto lvalue = equalityExpression();
        if (!match(COLON)) {
            error(peek(), "Missing token ':'");
        }
        const auto rvalue = equalityExpression();
        expr = new TernaryExpr(expr, lvalue, rvalue);
    }
    return expr;
}

Expr *Parser::equalityExpression() {
    auto expr = comparisonExpression();
    while (match({EQUAL_EQUAL, BANG_EQUAL})) {
        const auto op = previous();
        const auto rvalue = comparisonExpression();
        expr = new BinaryExpr(expr, op, rvalue);
    }
    return expr;
}

Expr *Parser::comparisonExpression() {
    auto expr = termExpression();
    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        const auto op = previous();
        const auto rvalue = termExpression();
        expr = new BinaryExpr(expr, op, rvalue);
    }
    return expr;
}

Expr *Parser::termExpression() {
    auto expr = factorExpression();
    while (match({PLUS, MINUS})) {
        const auto op = previous();
        const auto rvalue = factorExpression();
        expr = new BinaryExpr(expr, op, rvalue);
    }
    return expr;
}

Expr *Parser::factorExpression() {
    auto expr = unaryExpression();
    while (match({SLASH, STAR})) {
        const auto op = previous();
        const auto rvalue = unaryExpression();
        expr = new BinaryExpr(expr, op, rvalue);
    }
    return expr;
}

Expr *Parser::unaryExpression() {
    if (match({BANG, MINUS, PLUS})) {
        const auto op = previous();
        const auto rvalue = unaryExpression();
        return new UnaryExpr(rvalue, op);
    }
    return callExpression();
}

Expr *Parser::callExpression() {
    auto expr = primaryExpression();
    while (true) {
        if (match(L_PAREN)) {
            expr = finishCallExpr(expr);
        } else if (match(L_BRACKET)) {
            expr = finishIndexedCallExpr(expr);
        } else {
            break;
        }
    }
    return expr;
}

Expr *Parser::finishCallExpr(Expr *callee) {
    auto *args = new std::vector<Expr *>();
    if (!check(R_PAREN)) {
        do {
            args->push_back(expression());
        } while (match(COMMA));
    }
    const auto paren = consume(R_PAREN, "Expect '(' after parameters");
    return new CallExpr(callee, paren, args);
}

Expr *Parser::finishIndexedCallExpr(Expr *callee) {
    const auto index = expression();
    const auto bracket = consume(R_BRACKET, "Expect '[' after array index");
    return new IndexedCallExpr(callee, bracket, index);
}

Expr *Parser::primaryExpression() {
    Expr *expr = nullptr;
    if (match(TRUE) || match(FALSE) || match(STRING) || match(INT) || match(DOUBLE)) {
        expr = new LiteralExpr(previous());
    } else if (match(L_PAREN)) {
        const auto grouping = expression();
        consume(R_PAREN, "Missing ')'");
        expr = new GroupingExpr(grouping);
    } else if (match(IDENTIFIER)) {
        expr = new VariableExpr(previous());
    } else if (match(L_BRACKET)) {
        Token *bracket = previous();
        const auto elements = new std::vector<Expr *>();
        const auto arr = new ArrayExpr(bracket, elements);
        if (match(R_BRACKET)) {
            // Empty array
        } else {
            do {
                elements->push_back(expression());
            } while (match(COMMA));
            consume(R_BRACKET, "Missing ']'");
        }
        expr = arr;
    } else if (match(L_BRACE)) {
        Token *brace = previous();
        const auto elements = new std::vector<std::pair<Expr *, Expr *> >();
        const auto map = new MapExpr(brace, elements);
        if (match(R_BRACE)) {
            // Empty map
        } else {
            do {
                const auto key = expression();
                consume(COLON, "Missing ':'");
                const auto value = expression();
                elements->push_back(std::make_pair(key, value));
            } while (match(COMMA));
            consume(R_BRACE, "Missing '}'");
        }
        expr = map;
    } else {
        error(peek(), "Expect expression");
    }
    return expr;
}

Token *Parser::peek() const {
    return lexer->getTokenAt(currentIndex);
}

bool Parser::check(const TokenType expected) const {
    if (isAtEnd()) return false;
    return peek()->type() == expected;
}

Token *Parser::advance() {
    if (!isAtEnd()) ++currentIndex;
    return previous();
}

Token *Parser::previous() const {
    return lexer->getTokenAt(currentIndex - 1);
}

Token *Parser::consume(const TokenType type, const std::string &errorMessage) {
    if (check(type)) {
        return advance();
    }
    error(peek(), errorMessage);
    return nullptr;
}

void Parser::error(const Token *token, const std::string &message) {
    Logger::instance()->logError(token, message);
    throw ParserError();
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous()->type() == SEMICOLON) {
            return;
        }
        switch (peek()->type()) {
            case FUN:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case RETURN:
                return;
            default:
                advance();
        }
    }
}
