/**
 * @file program.h
 * @brief Program class declaration for nsbaci runtime service.
 *
 * This module defines the Program class which holds the compiled program's
 * instruction vector, memory tables, and other runtime data.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_SERVICES_RUNTIME_PROGRAM_H
#define NSBACI_SERVICES_RUNTIME_PROGRAM_H

#include <string>
#include <unordered_map>

#include "instruction.h"

/**
 * @namespace nsbaci::types
 * @brief Type definitions namespace for nsbaci.
 */
namespace nsbaci::types {

/// @brief Type alias for variable names
using VarName = std::string;

/// @brief Lookup table mapping variable names to their values
using VariableTable = std::unordered_map<VarName, int>;

}  // namespace nsbaci::types

/**
 * @namespace nsbaci::services::runtime
 * @brief Runtime services namespace for nsbaci.
 */
namespace nsbaci::services::runtime {

/**
 * @class Program
 * @brief Represents a compiled program ready for execution.
 *
 * The Program class contains the instruction vector, memory tables,
 * and other data structures needed for program execution.
 */
class Program {
 public:
  Program() = default;
  Program(nsbaci::types::InstructionStream i);
  ~Program() = default;

 private:
  // global lookup
  const nsbaci::types::InstructionStream instructions;
  nsbaci::types::VariableTable symbolTable;
};

}  // namespace nsbaci::services::runtime

#endif  // NSBACI_SERVICES_RUNTIME_PROGRAM_H