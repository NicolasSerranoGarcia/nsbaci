#pragma once

/**
 * @file lexer.h
 * @brief Lexer class declaration for nsbaci compiler.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "parser.tab.hh"

/**
 * @namespace nsbaci::compiler
 * @brief Compiler namespace for nsbaci.
 */
namespace nsbaci::compiler {

/**
 * @class Lexer
 * @brief Flex-based lexer for BACI source code.
 */
class Lexer : public yyFlexLexer {
 public:
  Lexer(std::istream* in = nullptr) : yyFlexLexer(in) {}
  virtual ~Lexer() = default;

  int yylex(Parser::semantic_type* yylval, Parser::location_type* yylloc);
};

}  // namespace nsbaci::compiler
