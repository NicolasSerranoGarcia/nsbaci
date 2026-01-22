/**
 * @file nsbaciCompiler.cpp
 * @brief NsbaciCompiler class implementation for nsbaci.
 *
 * This file contains the implementation of the NsbaciCompiler class which
 * uses flex-generated lexer and bison-generated parser to compile nsbaci
 * source code into p-code instructions for the virtual machine.
 *
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

namespace {

/**
 * @brief Converts the compiler's internal symbol table to runtime format.
 *
 * The parser uses an internal SymbolTable structure optimized for compilation.
 * This function converts it to the nsbaci::types::SymbolTable format used by
 * the runtime for variable display and debugging.
 *
 * @param st The compiler's internal symbol table.
 * @return SymbolTable in the runtime format with name, type, address info.
 */
nsbaci::types::SymbolTable convertSymbols(const SymbolTable& st) {
  nsbaci::types::SymbolTable result;
  for (const auto& [name, sym] : st.symbols) {
    nsbaci::types::SymbolInfo info;
    info.name = sym.name;
    info.address = sym.address;
    info.isGlobal = (sym.scopeLevel == 0);
    switch (sym.type) {
      case VarType::Int:
        info.type = "int";
        break;
      case VarType::Bool:
        info.type = "bool";
        break;
      case VarType::Char:
        info.type = "char";
        break;
      default:
        info.type = "void";
        break;
    }
    result[name] = info;
  }
  return result;
}

}  // namespace

CompilerResult NsbaciCompiler::compile(const std::string& source) {
  std::istringstream stream(source);
  return compile(stream);
}

CompilerResult NsbaciCompiler::compile(std::istream& input) {
  CompilerResult result;

  SymbolTable parserSymbols;  // Compiler's internal symbol table
  Lexer lexer(&input);
  Parser parser(lexer, result.instructions, result.errors, parserSymbols);

  int parseResult = parser.parse();

  // Set ok based on both parse result and whether errors were collected
  result.ok = (parseResult == 0) && result.errors.empty();

  // Convert parser's symbol table to runtime format
  if (result.ok) {
    result.symbols = convertSymbols(parserSymbols);
  }

  return result;
}

}  // namespace nsbaci::compiler
