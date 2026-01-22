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
  Program(nsbaci::compiler::InstructionStream i);
  ~Program() = default;

  Program(const Program&) = delete;
  Program& operator=(const Program&) = delete;

  Program(Program&&) = default;
  Program& operator=(Program&&) = default;

 private:
  // global lookup. A single instance of a program will have one an only one instruction stream. This acts as a read-only table. make the constructor take a rvalue, but as this is a typedef, I think it is overkill to make a strong tyoe and delete the copy constructor and operator. an rvalue should guarantee ownership of the object. Also, the public interface will not enable modifying this, so there should be no problem with immutability
  nsbaci::compiler::InstructionStream instructions;
  nsbaci::types::VariableTable symbolTable;
};

}  // namespace nsbaci::services::runtime

#endif  // NSBACI_SERVICES_RUNTIME_PROGRAM_H