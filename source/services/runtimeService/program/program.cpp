/**
 * @file program.cpp
 * @brief Program class implementation for nsbaci runtime service.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "program.h"

#include <stdexcept>

namespace nsbaci::services::runtime {

Program::Program(nsbaci::compiler::InstructionStream i)
    : instructions(std::move(i)) {}

Program::Program(nsbaci::compiler::InstructionStream i,
                 nsbaci::types::SymbolTable s)
    : instructions(std::move(i)), symbolTable(std::move(s)) {}

const nsbaci::compiler::Instruction& Program::getInstruction(
    uint32_t addr) const {
  if (addr >= instructions.size()) {
    throw std::out_of_range("Instruction address out of bounds");
  }
  return instructions[addr];
}

size_t Program::instructionCount() const { return instructions.size(); }

nsbaci::types::Memory& Program::memory() { return globalMemory; }

const nsbaci::types::Memory& Program::memory() const { return globalMemory; }

const nsbaci::types::SymbolTable& Program::symbols() const {
  return symbolTable;
}

void Program::addSymbol(nsbaci::types::SymbolInfo info) {
  symbolTable[info.name] = std::move(info);
}

int32_t Program::readMemory(nsbaci::types::MemoryAddr addr) const {
  if (addr >= globalMemory.size()) {
    return 0;  // Uninitialized memory reads as 0
  }
  return globalMemory[addr];
}

void Program::writeMemory(nsbaci::types::MemoryAddr addr, int32_t value) {
  if (addr >= globalMemory.size()) {
    globalMemory.resize(addr + 1, 0);
  }
  globalMemory[addr] = value;
}

}  // namespace nsbaci::services::runtime
