//
// Created by hhvvg on 8/28/24.
//

#ifndef EXPR_PARSER_HPP
#define EXPR_PARSER_HPP
#include "parser.hpp"

class ExprParser : public Parser {
public:
    explicit ExprParser(const std::vector<Token *> *tokens): Parser(tokens) {
    }

    Expr *parse() {
        return expression();
    }
};

#endif //EXPR_PARSER_HPP
