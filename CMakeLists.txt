cmake_minimum_required(VERSION 3.29)
project(soxsh)

set(CMAKE_CXX_STANDARD 20)

add_executable(soxsh main.cpp
        lexical/lexer.cpp
        lexical/lexer.hpp
        lexical/token_type.hpp
        lexical/token.hpp
        utils/utils.hpp
        lexical/value_holder.hpp
        utils/logger.hpp
        parser/expr.hpp
        parser/stmt.hpp
        parser/parser.cpp
        parser/parser.hpp
        utils/exception.hpp
        utils/logger.cpp
        interpret/interpreter.cpp
        interpret/interpreter.hpp
        interpret/runtime_scope.cpp
        interpret/runtime_scope.hpp
        interpret/callable.hpp
        interpret/resolver.cpp
        interpret/resolver.hpp
        interpret/builtin.hpp)
