/**
 * @file Instruction.h
 * @brief Base instruction class declaration for nsbaci runtime service.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_SERVICES_RUNTIME_INSTRUCTIONBASE_H
#define NSBACI_SERVICES_RUNTIME_INSTRUCTIONBASE_H

#include <vector>

#include "runtimeTypes.h"

/**
 * @namespace nsbaci::services::runtime
 * @brief Runtime services namespace for nsbaci.
 */
namespace nsbaci::services::runtime {

/**
 * @class Instruction
 * @brief Base class for all instructions in the runtime service.
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

}  // namespace nsbaci::services::runtime

/**
 * @namespace nsbaci::types
 * @brief Type definitions namespace for nsbaci.
 */
namespace nsbaci::types {

/// @brief Vector of instructions representing a compiled program
using InstructionStream = std::vector<nsbaci::services::runtime::Instruction>;

}  // namespace nsbaci::types

#endif  // NSBACI_SERVICES_RUNTIME_INSTRUCTIONBASE_H