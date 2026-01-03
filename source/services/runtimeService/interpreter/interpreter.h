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

#include "error.h"
#include "program.h"
#include "thread.h"

struct InterpreterResult {
  InterpreterResult() : ok(true) {}
  explicit InterpreterResult(std::vector<nsbaci::Error> errs)
      : ok(errs.empty()), errors(std::move(errs)) {}
  explicit InterpreterResult(nsbaci::Error error)
      : ok(false), errors({std::move(error)}) {}

  InterpreterResult(InterpreterResult&&) noexcept = default;
  InterpreterResult& operator=(InterpreterResult&&) noexcept = default;

  InterpreterResult(const InterpreterResult&) = default;
  InterpreterResult& operator=(const InterpreterResult&) = default;

  bool ok;
  std::vector<nsbaci::Error> errors;
  // additional info of executing an instruction from a thread
};

/**
 * @namespace nsbaci::services::runtime
 * @brief Runtime services namespace for nsbaci.
 */
namespace nsbaci::services::runtime {

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
};

}  // namespace nsbaci::services::runtime

#endif  // NSBACI_SERVICES_RUNTIME_INTERPRETER_H