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

#include <vector>

#include "error.h"
#include "fileTypes.h"
#include "instruction.h"
#include "runtimeTypes.h"

struct CompileResult {
  CompileResult() : ok(true) {}
  explicit CompileResult(std::vector<nsbaci::Error> errs)
      : ok(errs.empty()), errors(std::move(errs)) {}
  explicit CompileResult(nsbaci::Error error)
      : ok(false), errors({std::move(error)}) {}

  CompileResult(CompileResult&&) noexcept = default;
  CompileResult& operator=(CompileResult&&) noexcept = default;

  CompileResult(const CompileResult&) = default;
  CompileResult& operator=(const CompileResult&) = default;

  bool ok;
  std::vector<nsbaci::Error> errors;
  nsbaci::types::InstructionStream instructions;
};

/**
 * @namespace nsbaci::services
 * @brief Services namespace for nsbaci.
 */
namespace nsbaci::services {

/**
 * @class CompilerService
 */
class CompilerService {
 public:
  CompileResult compile(nsbaci::types::Text raw);

  CompilerService() = default;
  ~CompilerService() = default;
};

}  // namespace nsbaci::services

#endif  // NSBACI_COMPILERSERVICE_H
