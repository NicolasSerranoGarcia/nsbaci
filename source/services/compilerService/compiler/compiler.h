/**
 * @file compiler.h
 * @brief Abstract Compiler class declaration for nsbaci.
 *
 * This module defines the Compiler interface responsible for compiling
 * source code into an instruction stream.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_COMPILER_COMPILER_H
#define NSBACI_COMPILER_COMPILER_H

#include <sstream>
#include <string>
#include <vector>

#include "baseResult.h"
#include "instruction.h"

/**
 * @namespace nsbaci::compiler
 * @brief Compiler namespace for nsbaci.
 */
namespace nsbaci::compiler {

/**
 * @struct CompilerResult
 * @brief Result of a compilation operation.
 */
struct CompilerResult : nsbaci::BaseResult {
  CompilerResult() : BaseResult() {}
  explicit CompilerResult(std::vector<nsbaci::Error> errs)
      : BaseResult(std::move(errs)) {}
  explicit CompilerResult(nsbaci::Error error)
      : BaseResult(std::move(error)) {}

  CompilerResult(CompilerResult&&) noexcept = default;
  CompilerResult& operator=(CompilerResult&&) noexcept = default;

  CompilerResult(const CompilerResult&) = default;
  CompilerResult& operator=(const CompilerResult&) = default;

  InstructionStream instructions;
};

/**
 * @class Compiler
 * @brief Abstract base class for all compilers.
 *
 * The Compiler is responsible for parsing source code and generating
 * an instruction stream that can be executed by the runtime.
 */
class Compiler {
 public:
  Compiler() = default;
  virtual ~Compiler() = default;

  /**
   * @brief Compiles the given source code.
   * @param source The source code to compile.
   * @return CompilerResult containing the instruction stream or errors.
   */
  virtual CompilerResult compile(const std::string& source) = 0;

  /**
   * @brief Compiles from an input stream.
   * @param input The input stream to compile from.
   * @return CompilerResult containing the instruction stream or errors.
   */
  virtual CompilerResult compile(std::istream& input) = 0;
};

}  // namespace nsbaci::compiler

#endif  // NSBACI_COMPILER_COMPILER_H
