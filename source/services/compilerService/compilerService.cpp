/**
 * @file compilerService.cpp
 * @brief Implementation unit for the CompilerService.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "compilerService.h"

namespace nsbaci::services {

CompilerService::CompilerService(std::unique_ptr<nsbaci::compiler::Compiler> c)
    : compiler(std::move(c)) {}

nsbaci::compiler::CompilerResult CompilerService::compile(
    nsbaci::types::Text raw) {
  return compiler->compile(raw);
}

}  // namespace nsbaci::services
