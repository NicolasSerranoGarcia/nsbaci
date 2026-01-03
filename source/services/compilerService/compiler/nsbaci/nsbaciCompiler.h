/**
 * @file nsbaciCompiler.h
 * @brief NsbaciCompiler class declaration for nsbaci.
 *
 * This module provides the concrete BACI-specific implementation of the
 * Compiler interface for compiling BACI source code.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_COMPILER_NSBACI_COMPILER_H
#define NSBACI_COMPILER_NSBACI_COMPILER_H

#include "compiler.h"

/**
 * @namespace nsbaci::compiler
 * @brief Compiler namespace for nsbaci.
 */
namespace nsbaci::compiler {

/**
 * @class NsbaciCompiler
 * @brief BACI-specific implementation of the Compiler.
 *
 * NsbaciCompiler uses flex and bison to parse BACI source code
 * and generate an instruction stream for execution.
 */
class NsbaciCompiler final : public Compiler {
 public:
  NsbaciCompiler() = default;
  ~NsbaciCompiler() override = default;

  /**
   * @brief Compiles the given source code.
   * @param source The source code to compile.
   * @return CompilerResult containing the instruction stream or errors.
   */
  CompilerResult compile(const std::string& source) override;

  /**
   * @brief Compiles from an input stream.
   * @param input The input stream to compile from.
   * @return CompilerResult containing the instruction stream or errors.
   */
  CompilerResult compile(std::istream& input) override;

 private:
};

}  // namespace nsbaci::compiler

#endif  // NSBACI_COMPILER_NSBACI_COMPILER_H
