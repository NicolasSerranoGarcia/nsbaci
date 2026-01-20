/**
 * @file compilerService.h
 * @brief CompilerService class declaration for nsbaci.
 *
 * This service is intended to encapsulate compilation-related logic.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_COMPILERSERVICE_H
#define NSBACI_COMPILERSERVICE_H

#include <memory>
#include <vector>

#include "compiler.h"
#include "error.h"
#include "fileTypes.h"

/**
 * @namespace nsbaci::services
 * @brief Services namespace for nsbaci.
 */
namespace nsbaci::services {

/**
 * @class CompilerService
 * @brief Service that wraps the compiler for use in the application.
 */
class CompilerService {
 public:
  CompilerService();
  CompilerService(std::unique_ptr<nsbaci::compiler::Compiler> c);
  ~CompilerService() = default;

  nsbaci::compiler::CompilerResult compile(nsbaci::types::Text raw);

 private:
  std::unique_ptr<nsbaci::compiler::Compiler> compiler;
};

}  // namespace nsbaci::services

#endif  // NSBACI_COMPILERSERVICE_H
