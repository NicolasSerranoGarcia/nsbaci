/**
 * @file interpreter.h
 * @brief Interpreter class declaration for nsbaci runtime service.
 *
 * This module defines the Interpreter class responsible for executing
 * instructions within a program context.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_SERVICES_RUNTIME_INTERPRETER_H
#define NSBACI_SERVICES_RUNTIME_INTERPRETER_H

#include <functional>
#include <optional>
#include <string>

#include "baseResult.h"
#include "program.h"
#include "thread.h"

struct InterpreterResult : nsbaci::BaseResult {
  InterpreterResult() : BaseResult() {}
  explicit InterpreterResult(std::vector<nsbaci::Error> errs)
      : BaseResult(std::move(errs)) {}
  explicit InterpreterResult(nsbaci::Error error)
      : BaseResult(std::move(error)) {}

  InterpreterResult(InterpreterResult&&) noexcept = default;
  InterpreterResult& operator=(InterpreterResult&&) noexcept = default;

  InterpreterResult(const InterpreterResult&) = default;
  InterpreterResult& operator=(const InterpreterResult&) = default;

  bool needsInput = false;  ///< Thread is waiting for input
  std::string inputPrompt;  ///< Prompt to show for input
  std::string output;       ///< Output produced by this instruction
};

/**
 * @namespace nsbaci::services::runtime
 * @brief Runtime services namespace for nsbaci.
 */
namespace nsbaci::services::runtime {

/// @brief Callback type for output operations
using OutputCallback = std::function<void(const std::string&)>;

/// @brief Callback type for input requests
using InputRequestCallback = std::function<void(const std::string&)>;

/**
 * @class Interpreter
 * @brief Executes instructions for threads within a program context.
 *
 * The Interpreter is responsible for fetching and executing instructions
 * based on the current thread's program counter.
 */
class Interpreter {
 public:
  Interpreter() = default;
  virtual ~Interpreter() = default;

  /**
   * @brief Executes the current instruction for the given thread with the
   * program context.
   * @param t The thread whose instruction should be executed.
   * @param program The program context in which to execute the instruction
   */
  virtual InterpreterResult executeInstruction(Thread& t, Program& program) = 0;

  /**
   * @brief Provide input to a thread waiting for input.
   * @param input The input string.
   */
  virtual void provideInput(const std::string& input) = 0;

  /**
   * @brief Check if interpreter is waiting for input.
   * @return True if waiting for input.
   */
  virtual bool isWaitingForInput() const = 0;

  /**
   * @brief Set the output callback for print operations.
   * @param callback Function to call when output is produced.
   */
  virtual void setOutputCallback(OutputCallback callback) = 0;
};

}  // namespace nsbaci::services::runtime

#endif  // NSBACI_SERVICES_RUNTIME_INTERPRETER_H