/**
 * @file instruction.h
 * @brief Base instruction class declaration for nsbaci compiler.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_COMPILER_INSTRUCTION_H
#define NSBACI_COMPILER_INSTRUCTION_H

#include <memory>
#include <vector>

#include "runtimeTypes.h"

/**
 * @namespace nsbaci::compiler
 * @brief Compiler namespace for nsbaci.
 */
namespace nsbaci::compiler {

/**
 * @class Instruction
 * @brief Base class for all instructions in the compiler.
 */
class Instruction {
 public:
  Instruction() = default;
  virtual ~Instruction() = default;

  /**
   * @brief Executes the instruction.
   */
  virtual void execute() = 0;

 private:
};

/// @brief Unique pointer to an instruction
using InstructionPtr = std::unique_ptr<Instruction>;

/// @brief Vector of instructions representing a compiled program
using InstructionStream = std::vector<InstructionPtr>;

}  // namespace nsbaci::compiler

#endif  // NSBACI_COMPILER_INSTRUCTION_H
