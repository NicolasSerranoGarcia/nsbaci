/**
 * @file compilerService.cpp
 * @brief Implementation of the CompilerService class for nsbaci.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "compilerService.h"

#include <algorithm>
#include <cctype>

namespace nsbaci::services {

CompilerService::CompilerService(std::unique_ptr<nsbaci::compiler::Compiler> c)
    : compiler(std::move(c)) {}

nsbaci::compiler::CompilerResult CompilerService::compile(
    nsbaci::types::Text raw) {
  // Check if input is empty or contains only whitespace
  bool isEmpty = raw.empty() ||
                 std::all_of(raw.begin(), raw.end(),
                             [](unsigned char c) { return std::isspace(c); });
  if (isEmpty) {
    nsbaci::Error err;
    err.basic.severity = nsbaci::types::ErrSeverity::Error;
    err.basic.message = "Cannot compile empty file";
    err.basic.type = nsbaci::types::ErrType::compilationError;
    err.payload = nsbaci::types::CompileError{1, 1};
    programReady = false;
    return nsbaci::compiler::CompilerResult(std::move(err));
  }

  auto result = compiler->compile(raw);
  if (result.ok) {
    lastCompiledInstructions = std::move(result.instructions);
    lastCompiledSymbols = std::move(result.symbols);
    programReady = true;
  } else {
    programReady = false;
  }

  return result;
}

bool CompilerService::hasProgramReady() const { return programReady; }

nsbaci::compiler::InstructionStream CompilerService::takeInstructions() {
  programReady = false;
  return std::move(lastCompiledInstructions);
}

nsbaci::types::SymbolTable CompilerService::takeSymbols() {
  return std::move(lastCompiledSymbols);
}

}  // namespace nsbaci::services
