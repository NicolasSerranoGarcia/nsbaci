/**
 * @file compilerService.h
 * @brief CompilerService class declaration for nsbaci.
 *
 * This module defines the CompilerService class that provides a high-level
 * interface for compiling nsbaci source code into executable p-code
 * instructions. The service wraps a Compiler implementation and manages the
 * compiled program state, making it available for the runtime system.
 *
 * The CompilerService acts as a bridge between the Controller and the actual
 * compiler implementation. This means that the compilerService holds a specific
 * compiler, and bubbles the interface of the compiler itself
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
#include "nsbaciCompiler.h"

/**
 * @namespace nsbaci::services
 * @brief Services namespace containing all backend service implementations.
 */
namespace nsbaci::services {

/**
 * @class CompilerService
 * @brief service for compiling nsbaci (or maybe other stuff in the future)
 * source code.
 *
 *
 * The service functions like the following:
 * 1. Call compile() with source code
 * 2. Check hasProgramReady() to verify success
 * 3. Call takeInstructions() and takeSymbols() to retrieve compiled data
 *
 * After taking the instructions, the program is no longer considered ready
 * until a new successful compilation occurs.
 *
 * Usage example:
 * @code
 * CompilerService cs;
 * auto result = cs.compile(sourceCode);
 * if (result.ok && cs.hasProgramReady()) {
 *   auto instructions = cs.takeInstructions();
 *   auto symbols = cs.takeSymbols();
 *   // Load into runtime...
 * }
 * @endcode
 */
class CompilerService {
 public:
  /**
   * @brief Constructs the CompilerService with a compiler implementation.
   *
   * By default, creates an NsbaciCompiler instance. Custom compiler
   * implementations can be injected for testing or alternative language
   * support.
   *
   * @param c Unique pointer to a Compiler implementation. Defaults to
   * NsbaciCompiler.
   */
  CompilerService(std::unique_ptr<nsbaci::compiler::Compiler> c =
                      std::make_unique<nsbaci::compiler::NsbaciCompiler>());

  /**
   * @brief Default destructor.
   */
  ~CompilerService() = default;

  CompilerService(const CompilerService&) = delete;
  CompilerService& operator=(const CompilerService&) = delete;

  CompilerService(CompilerService&&) = default;
  CompilerService& operator=(CompilerService&&) = default;

  /**
   * @brief Compiles nsbaci source code into p-code instructions.
   *
   * Passes the source code to the underlying compiler and stores the results
   * if compilation succeeds. The compiled instructions and symbols can then
   * be retrieved using takeInstructions() and takeSymbols().
   *
   * @param raw The nsbaci source code to compile.
   * @return CompilerResult containing success status and any compilation
   * errors.
   */
  nsbaci::compiler::CompilerResult compile(nsbaci::types::Text raw);

  /**
   * @brief Checks if a compiled program is available for execution.
   *
   * Returns true after a successful compile() call and before
   * takeInstructions() is called. Used to verify that valid instructions are
   * available before attempting to load them into the runtime.
   *
   * @return True if compiled instructions are available, false otherwise.
   */
  bool hasProgramReady() const;

  /**
   * @brief Retrieves and releases ownership of the compiled instructions.
   *
   * Moves the instruction stream out of the service. After this call,
   * hasProgramReady() will return false until a new successful compilation.
   *
   * @return The compiled instruction stream.
   * @warning Only call when hasProgramReady() returns true.
   */
  nsbaci::compiler::InstructionStream takeInstructions();

  /**
   * @brief Retrieves and releases ownership of the symbol table.
   *
   * Moves the symbol table out of the service. The symbol table contains
   * information about all declared variables including names, types, and
   * memory addresses.
   *
   * @return The symbol table from the last successful compilation.
   */
  nsbaci::types::SymbolTable takeSymbols();

 private:
  std::unique_ptr<nsbaci::compiler::Compiler>
      compiler;  ///< The underlying compiler implementation.
  nsbaci::compiler::InstructionStream
      lastCompiledInstructions;  ///< Stored instructions from last compile.
  nsbaci::types::SymbolTable
      lastCompiledSymbols;    ///< Stored symbols from last compile.
  bool programReady = false;  ///< True if valid compiled program is available.
};

}  // namespace nsbaci::services

#endif  // NSBACI_COMPILERSERVICE_H
