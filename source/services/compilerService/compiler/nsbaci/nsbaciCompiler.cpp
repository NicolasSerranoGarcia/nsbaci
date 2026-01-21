/**
 * @file nsbaciCompiler.cpp
 * @brief NsbaciCompiler class implementation for nsbaci.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "nsbaciCompiler.h"

#include <sstream>

#include "lexer.h"
#include "parser.tab.hh"

namespace nsbaci::compiler {

CompilerResult NsbaciCompiler::compile(const std::string& source) {
  std::istringstream stream(source);
  return compile(stream);
}

CompilerResult NsbaciCompiler::compile(std::istream& input) {
  CompilerResult result;

  Lexer lexer(&input);
  Parser parser(lexer, result.instructions);

  if (parser.parse() != 0) {
    result.ok = false;
    // Errors are collected during parsing
  }

  return result;
}

}  // namespace nsbaci::compiler
