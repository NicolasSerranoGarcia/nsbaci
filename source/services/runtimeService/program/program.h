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
#include <vector>

#include "compilerTypes.h"
#include "instruction.h"

/**
 * @namespace nsbaci::types
 * @brief Type definitions namespace for nsbaci (runtime-specific).
 */
namespace nsbaci::types {

/// @brief Stack value type (can hold int or address)
using StackValue = int32_t;

/// @brief Runtime stack
using Stack = std::vector<StackValue>;

/// @brief Memory block for runtime data
using Memory = std::vector<int32_t>;

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
  explicit Program(nsbaci::compiler::InstructionStream i);
  Program(nsbaci::compiler::InstructionStream i, nsbaci::types::SymbolTable s);
  ~Program() = default;

  Program(const Program&) = delete;
  Program& operator=(const Program&) = delete;

  Program(Program&&) = default;
  Program& operator=(Program&&) = default;

  /**
   * @brief Gets instruction at the given address.
   * @param addr The instruction address.
   * @return Reference to the instruction.
   */
  const nsbaci::compiler::Instruction& getInstruction(uint32_t addr) const;

  /**
   * @brief Gets the total number of instructions.
   * @return Number of instructions in the program.
   */
  size_t instructionCount() const;

  /**
   * @brief Access to global memory.
   * @return Reference to memory.
   */
  nsbaci::types::Memory& memory();
  const nsbaci::types::Memory& memory() const;

  /**
   * @brief Access to symbol table.
   * @return Const reference to symbol table.
   */
  const nsbaci::types::SymbolTable& symbols() const;

  /**
   * @brief Add a symbol to the symbol table.
   * @param info The symbol information to add.
   */
  void addSymbol(nsbaci::types::SymbolInfo info);

  /**
   * @brief Read a value from memory.
   * @param addr Memory address to read from.
   * @return Value at the address.
   */
  int32_t readMemory(nsbaci::types::MemoryAddr addr) const;

  /**
   * @brief Write a value to memory.
   * @param addr Memory address to write to.
   * @param value Value to write.
   */
  void writeMemory(nsbaci::types::MemoryAddr addr, int32_t value);

 private:
  // Instruction stream - read-only after construction
  nsbaci::compiler::InstructionStream instructions;
  // Global symbol table
  nsbaci::types::SymbolTable symbolTable;
  // Global memory
  nsbaci::types::Memory globalMemory;
};

}  // namespace nsbaci::services::runtime

#endif  // NSBACI_SERVICES_RUNTIME_PROGRAM_H