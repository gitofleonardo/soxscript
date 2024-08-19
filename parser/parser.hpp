//
// Created by hhvvg on 8/18/24.
//

#ifndef PARSER_HPP
#define PARSER_HPP
#include <sys/types.h>

#include "stmt.hpp"
#include "../lexical/lexer.hpp"

class Parser {
    ulong currentIndex = 0;
    const Lexer *lexer;

    [[nodiscard]] Token *peek() const;

    [[nodiscard]] bool check(TokenType expected) const;

    [[nodiscard]] bool isAtEnd() const;

    Token *advance();

    [[nodiscard]] Token *previous() const;

    Token *consume(TokenType type, const std::string &errorMessage);

    static void error(const Token *token, const std::string &message);

    void synchronize();

    bool match(std::initializer_list<TokenType>);

    bool match(TokenType expected);

    Expr *finishCallExpr(Expr *callee);

public:
    explicit Parser(const Lexer *lexer);

    ~Parser();

    std::vector<Stmt *> *parse();

    [[nodiscard]] Stmt *statement();

    [[nodiscard]] Stmt *returnStatement();

    [[nodiscard]] Stmt *forStatement();

    [[nodiscard]] Stmt *whileStatement();

    [[nodiscard]] Stmt *ifStatement();

    [[nodiscard]] Stmt *declarationStatement();

    [[nodiscard]] Stmt *functionDeclarationStatement();

    [[nodiscard]] Stmt *variableDeclarationStatement();

    [[nodiscard]] Stmt *expressionStatement();

    [[nodiscard]] Stmt *blockStatement();

    [[nodiscard]] Expr *expression();

    [[nodiscard]] Expr *assignment();

    [[nodiscard]] Expr *orExpression();

    [[nodiscard]] Expr *andExpression();

    [[nodiscard]] Expr *ternaryExpression();

    [[nodiscard]] Expr *equalityExpression();

    [[nodiscard]] Expr *comparisonExpression();

    [[nodiscard]] Expr *termExpression();

    [[nodiscard]] Expr *factorExpression();

    [[nodiscard]] Expr *unaryExpression();

    [[nodiscard]] Expr *callExpression();

    [[nodiscard]] Expr *primaryExpression();
};

#endif //PARSER_HPP
