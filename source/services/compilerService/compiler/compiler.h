/**
 * @file compiler.h
 * @brief Abstract Compiler class declaration for nsbaci.
 *
 * This module defines the abstract Compiler interface that all compiler
 * implementations must follow. The Compiler is responsible for transforming
 * nsbaci source code into an executable instruction stream (p-code) that can
 * be run by the virtual machine interpreter.
 *
 * The design follows the Strategy pattern, allowing different compiler
 * implementations to be plugged in. Currently, NsbaciCompiler is the primary
 * implementation using flex/bison for lexical analysis and parsing.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_COMPILER_COMPILER_H
#define NSBACI_COMPILER_COMPILER_H

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "baseResult.h"
#include "compilerTypes.h"
#include "instruction.h"

/**
 * @namespace nsbaci::compiler
 * @brief Compiler namespace containing all compilation-related stuff.
 */
namespace nsbaci::compiler {

/**
 * @struct CompilerResult
 * @brief Result of a compilation operation.
 *
 * Contains the outcome of a compilation attempt including success/failure
 * status, any error messages, and on success, the generated instruction
 * stream and symbol table.
 *
 * @note On failure, the instructions and symbols fields should not be used.
 */
struct CompilerResult : nsbaci::BaseResult {
  /**
   * @brief Default constructor creates a successful empty result.
   */
  CompilerResult() : BaseResult() {}

  /**
   * @brief Constructs a failed result from a vector of errors.
   * @param errs Vector of compilation errors.
   */
  explicit CompilerResult(std::vector<nsbaci::Error> errs)
      : BaseResult(std::move(errs)) {}

  /**
   * @brief Constructs a failed result from a single error.
   * @param error The compilation error.
   */
  explicit CompilerResult(nsbaci::Error error) : BaseResult(std::move(error)) {}

  CompilerResult(CompilerResult&&) noexcept = default;
  CompilerResult& operator=(CompilerResult&&) noexcept = default;

  CompilerResult(const CompilerResult&) = default;
  CompilerResult& operator=(const CompilerResult&) = default;

  InstructionStream instructions;      ///< Generated p-code instructions.
  nsbaci::types::SymbolTable symbols;  ///< Symbol table from compilation.
};

/**
 * @class Compiler
 * @brief Abstract base class for all compilers.
 *
 * The Compiler interface defines the contract for compiling source code
 * into p-code instructions. Implementations handle lexical analysis, parsing,
 * semantic analysis, and code generation.
 *
 * The compilation process produces:
 * - An instruction stream (p-code) for the virtual machine
 * - A symbol table mapping variable names to their types and addresses
 * - Compilation errors, if there are
 *
 * Subclasses must implement both compile() overloads to support compilation
 * from both strings and input streams.
 */
class Compiler {
 public:
  /**
   * @brief Default constructor.
   */
  Compiler() = default;

  /**
   * @brief Virtual destructor.
   */
  virtual ~Compiler() = default;

  /**
   * @brief Compiles source code from a string.
   *
   * Performs full compilation including lexical analysis, parsing,
   * semantic analysis, and p-code generation.
   *
   * @param source The BACI source code to compile.
   * @return CompilerResult containing instructions on success, or errors on
   * failure.
   */
  virtual CompilerResult compile(const std::string& source) = 0;

  /**
   * @brief Compiles source code from an input stream.
   *
   * Allows compilation from files or other stream sources.
   *
   * @param input The input stream containing BACI source code.
   * @return CompilerResult containing instructions on success, or errors on
   * failure.
   */
  virtual CompilerResult compile(std::istream& input) = 0;
};

}  // namespace nsbaci::compiler

#endif  // NSBACI_COMPILER_COMPILER_H
