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

#include "runtimeTypes.h"

using namespace nsbaci::types;

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

#endif  // NSBACI_SERVICES_RUNTIME_INSTRUCTIONBASE_H