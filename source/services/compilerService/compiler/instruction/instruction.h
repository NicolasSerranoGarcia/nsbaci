/**
 * @file instruction.h
 * @brief Instruction definitions for nsbaci compiler.
 *
 * This module defines the instruction set for the BACI virtual machine,
 * including opcodes and instruction representation.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_COMPILER_INSTRUCTION_H
#define NSBACI_COMPILER_INSTRUCTION_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

/**
 * @namespace nsbaci::compiler
 * @brief Compiler namespace for nsbaci.
 */
namespace nsbaci::compiler {

/**
 * @enum Opcode
 * @brief Opcodes for the BACI virtual machine instruction set.
 */
enum class Opcode : uint8_t {
  // ============== Stack/Memory Operations ==============
  LoadValue,      // Load value from address onto stack
  LoadAddress,    // Load address onto stack
  LoadIndirect,   // Load value from address pointed to by top of stack
  LoadBlock,      // Load block of memory onto stack
  Store,          // Store top of stack to address
  StoreKeep,      // Store and keep value on stack
  PushLiteral,    // Push literal value onto stack
  Index,          // Array indexing
  CopyBlock,      // Copy block of memory
  ValueAt,        // Get value at address on stack
  MarkStack,      // Mark stack for procedure call
  UpdateDisplay,  // Update display register

  // ============== Arithmetic Operations ==============
  Add,         // Addition
  Sub,         // Subtraction
  Mult,        // Multiplication
  Div,         // Integer division
  Mod,         // Modulo
  Negate,      // Unary negation
  Complement,  // Bitwise complement

  // ============== Logical Operations ==============
  And,  // Logical AND
  Or,   // Logical OR

  // ============== Comparison Operations ==============
  TestEQ,         // Test equal
  TestNE,         // Test not equal
  TestLT,         // Test less than
  TestLE,         // Test less or equal
  TestGT,         // Test greater than
  TestGE,         // Test greater or equal
  TestEqualKeep,  // Test equal, keep operands

  // ============== Control Flow ==============
  Jump,          // Unconditional jump
  JumpZero,      // Jump if top of stack is zero
  Call,          // Call procedure
  ShortCall,     // Short call (no display update)
  ShortReturn,   // Short return
  ExitProc,      // Exit procedure
  ExitFunction,  // Exit function (with return value)
  Halt,          // Halt execution

  // ============== Loop Control ==============
  BeginFor,  // Begin for loop
  EndFor,    // End for loop

  // ============== Concurrency - Process ==============
  Cobegin,    // Begin concurrent block
  Coend,      // End concurrent block
  Create,     // Create new process
  Suspend,    // Suspend current process
  Revive,     // Revive suspended process
  WhichProc,  // Get current process ID

  // ============== Concurrency - Semaphores ==============
  Wait,            // Wait on semaphore (P operation)
  Signal,          // Signal semaphore (V operation)
  StoreSemaphore,  // Initialize semaphore

  // ============== Concurrency - Monitors ==============
  EnterMonitor,       // Enter monitor
  ExitMonitor,        // Exit monitor
  CallMonitorInit,    // Call monitor initialization
  ReturnMonitorInit,  // Return from monitor init
  WaitCondition,      // Wait on condition variable
  SignalCondition,    // Signal condition variable
  Empty,              // Check if condition queue is empty

  // ============== I/O Operations ==============
  Read,            // Read integer
  Readln,          // Read line
  Write,           // Write value
  Writeln,         // Write newline
  WriteString,     // Write string
  WriteRawString,  // Write raw string literal
  EolEof,          // Check end of line/file
  Sprintf,         // Format string
  Sscanf,          // Scan string

  // ============== String Operations ==============
  CopyString,        // Copy string
  CopyRawString,     // Copy raw string
  ConcatString,      // Concatenate strings
  ConcatRawString,   // Concatenate raw string
  CompareString,     // Compare strings
  CompareRawString,  // Compare raw strings
  LengthString,      // Get string length

  // ============== Graphics Operations ==============
  MoveTo,       // Move to absolute position
  MoveBy,       // Move by relative offset
  ChangeColor,  // Change drawing color
  MakeVisible,  // Make object visible
  Remove,       // Remove object

  // ============== Miscellaneous ==============
  Random,  // Generate random number
  Test,    // Generic test instruction

  // ============== Total count ==============
  _Count  // Number of opcodes (keep last)
};

/**
 * @brief Operand types that an instruction can have.
 */
using Operand = std::variant<std::monostate,  // No operand
                             int32_t,         // Integer literal or offset
                             uint32_t,        // Unsigned value or address
                             std::string      // String literal
                             >;

/**
 * @struct Instruction
 * @brief Represents a single instruction in the virtual machine.
 */
struct Instruction {
  Opcode opcode;
  Operand operand1;
  Operand operand2;

  // Convenience constructors
  Instruction() : opcode(Opcode::Halt), operand1(), operand2() {}

  explicit Instruction(Opcode op) : opcode(op), operand1(), operand2() {}

  Instruction(Opcode op, int32_t op1) : opcode(op), operand1(op1), operand2() {}

  Instruction(Opcode op, uint32_t op1)
      : opcode(op), operand1(op1), operand2() {}

  Instruction(Opcode op, std::string op1)
      : opcode(op), operand1(std::move(op1)), operand2() {}

  Instruction(Opcode op, int32_t op1, int32_t op2)
      : opcode(op), operand1(op1), operand2(op2) {}
};

/// @brief Vector of instructions representing a compiled program
using InstructionStream = std::vector<Instruction>;

/**
 * @brief Get the string name of an opcode (for debugging/display).
 * @param op The opcode to convert.
 * @return String representation of the opcode.
 */
const char* opcodeName(Opcode op);

}  // namespace nsbaci::compiler

#endif  // NSBACI_COMPILER_INSTRUCTION_H
