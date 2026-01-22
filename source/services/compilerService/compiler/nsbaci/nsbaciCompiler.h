/**
 * @file nsbaciCompiler.h
 * @brief NsbaciCompiler class declaration for nsbaci.
 *
 * This module provides the concrete nsbaci implementation of the
 * Compiler interface. NsbaciCompiler uses flex for lexical analysis and
 * bison for parsing to compile BACI source code into p-code instructions.
 *
 * The compiler supports:
 * - Basic types: int, bool, char
 * - Arithmetic and comparison operators
 * - Control flow: if/else, while, for loops
 * - C++ style I/O: cout << and cin >>
 * - Variable declarations with optional initialization
 * - Compound assignment operators (+=, -=, etc.)
 *
 * Future features (not yet implemented):
 * - Functions and procedures
 * - Concurrency primitives (cobegin/coend, semaphores, monitors)
 * - Arrays and strings
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
 * @brief Compiler namespace containing all compilation-related stuff.
 */
namespace nsbaci::compiler {

/**
 * @class NsbaciCompiler
 * @brief nsbaci compiler implementation using flex and bison.
 *
 * Usage example:
 * @code
 * NsbaciCompiler compiler;
 * auto result = compiler.compile("int x = 5; cout << x << endl;");
 * if (result.ok) {
 *   // result.instructions contains the p-code
 *   // result.symbols contains variable information
 * } else {
 *   // Handle compilation errors
 * }
 * @endcode
 */
class NsbaciCompiler final : public Compiler {
 public:
  /**
   * @brief Default constructor.
   */
  NsbaciCompiler() = default;

  /**
   * @brief Destructor.
   */
  ~NsbaciCompiler() override = default;

  /**
   * @brief Compiles nsbaci source code from a string.
   *
   * Creates a string stream from the source and delegates to the
   * stream compile method.
   *
   * @param source The nsbaci source code to compile.
   * @return CompilerResult with instructions and symbols on success,
   *         or detailed error information on failure.
   */
  CompilerResult compile(const std::string& source) override;

  /**
   * @brief Compiles nsbaci source code from an input stream.
   *
   * Performs the actual compilation by:
   * 1. Creating a Lexer to tokenize the input
   * 2. Creating a Parser with the lexer
   * 3. Running the parser to generate instructions
   * 4. Collecting any errors that occurred
   *
   * @param input The input stream containing nsbaci source code.
   * @return CompilerResult with instructions and symbols on success,
   *         or detailed error information on failure.
   */
  CompilerResult compile(std::istream& input) override;
};

}  // namespace nsbaci::compiler

#endif  // NSBACI_COMPILER_NSBACI_COMPILER_H
