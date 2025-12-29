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

#include "runtimeTypes.h"
#include "program.h"
#include "thread.h"

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
    ~Interpreter() = default;

    /**
     * @brief Executes the current instruction for the given thread.
     * @param t The thread whose instruction should be executed.
     */
    void executeInstruction(Thread& t);

    void changeProgram(Program p);

 private:
    Program program;  ///< Current running program with instruction vector and memory tables
};

}  // namespace nsbaci::services::runtime

#endif  // NSBACI_SERVICES_RUNTIME_INTERPRETER_H