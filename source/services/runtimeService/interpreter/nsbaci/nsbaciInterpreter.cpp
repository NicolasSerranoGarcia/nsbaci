/**
 * @file nsbaciInterpreter.cpp
 * @brief NsbaciInterpreter class implementation for nsbaci runtime service.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "nsbaciInterpreter.h"

#include "instruction.h"

namespace nsbaci::services::runtime {

InterpreterResult NsbaciInterpreter::executeInstruction(Thread& t,
                                                        Program& program) {
  using namespace nsbaci::compiler;
  InterpreterResult result;

  // Fetch instruction
  const uint32_t pc = t.getPC();
  if (pc >= program.instructionCount()) {
    nsbaci::Error err;
    err.basic.severity = nsbaci::types::ErrSeverity::Fatal;
    err.basic.message = "Program counter out of bounds";
    err.basic.type = nsbaci::types::ErrType::unknown;
    err.payload = nsbaci::types::RuntimeError{};
    return InterpreterResult(std::move(err));
  }

  const Instruction& instr = program.getInstruction(pc);
  bool advancePC = true;  // Most instructions advance PC

  // Decode and execute
  switch (instr.opcode) {
    // ============== Stack/Memory Operations ==============
    case Opcode::PushLiteral: {
      int32_t value = std::get<int32_t>(instr.operand1);
      t.push(value);
      break;
    }

    case Opcode::Store: {
      // Address is in operand1, value is on stack
      uint32_t addr = std::get<uint32_t>(instr.operand1);
      int32_t value = t.pop();
      if (addr >= program.memory().size()) {
        program.memory().resize(addr + 1, 0);
      }
      program.memory()[addr] = value;
      break;
    }

    case Opcode::StoreKeep: {
      // Like Store but keeps the value on the stack
      uint32_t addr = std::get<uint32_t>(instr.operand1);
      int32_t value = t.top();
      if (addr >= program.memory().size()) {
        program.memory().resize(addr + 1, 0);
      }
      program.memory()[addr] = value;
      break;
    }

    case Opcode::LoadValue: {
      // Address is in operand1
      uint32_t addr = std::get<uint32_t>(instr.operand1);
      if (addr >= program.memory().size()) {
        t.push(0);  // Uninitialized memory reads as 0
      } else {
        t.push(program.memory()[addr]);
      }
      break;
    }

    case Opcode::LoadAddress: {
      uint32_t addr = std::get<uint32_t>(instr.operand1);
      t.push(static_cast<int32_t>(addr));
      break;
    }

    case Opcode::LoadIndirect: {
      // Address is on stack, load value from that address
      uint32_t addr = static_cast<uint32_t>(t.pop());
      if (addr >= program.memory().size()) {
        t.push(0);
      } else {
        t.push(program.memory()[addr]);
      }
      break;
    }

    // ============== Arithmetic Operations ==============
    case Opcode::Add: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      t.push(a + b);
      break;
    }

    case Opcode::Sub: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      t.push(a - b);
      break;
    }

    case Opcode::Mult: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      t.push(a * b);
      break;
    }

    case Opcode::Div: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      if (b == 0) {
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Division by zero";
        err.basic.type = nsbaci::types::ErrType::unknown;
        err.payload = nsbaci::types::RuntimeError{};
        return InterpreterResult(std::move(err));
      }
      t.push(a / b);
      break;
    }

    case Opcode::Mod: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      if (b == 0) {
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Modulo by zero";
        err.basic.type = nsbaci::types::ErrType::unknown;
        err.payload = nsbaci::types::RuntimeError{};
        return InterpreterResult(std::move(err));
      }
      t.push(a % b);
      break;
    }

    case Opcode::Negate: {
      int32_t a = t.pop();
      t.push(-a);
      break;
    }

    // ============== Logical Operations ==============
    case Opcode::And: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      t.push((a != 0 && b != 0) ? 1 : 0);
      break;
    }

    case Opcode::Or: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      t.push((a != 0 || b != 0) ? 1 : 0);
      break;
    }

    // ============== Comparison Operations ==============
    case Opcode::TestEQ: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      t.push(a == b ? 1 : 0);
      break;
    }

    case Opcode::TestNE: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      t.push(a != b ? 1 : 0);
      break;
    }

    case Opcode::TestLT: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      t.push(a < b ? 1 : 0);
      break;
    }

    case Opcode::TestLE: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      t.push(a <= b ? 1 : 0);
      break;
    }

    case Opcode::TestGT: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      t.push(a > b ? 1 : 0);
      break;
    }

    case Opcode::TestGE: {
      int32_t b = t.pop();
      int32_t a = t.pop();
      t.push(a >= b ? 1 : 0);
      break;
    }

    // ============== Control Flow ==============
    case Opcode::Jump: {
      int32_t target = std::get<int32_t>(instr.operand1);
      t.setPC(static_cast<uint32_t>(target));
      advancePC = false;
      break;
    }

    case Opcode::JumpZero: {
      int32_t cond = t.pop();
      if (cond == 0) {
        int32_t target = std::get<int32_t>(instr.operand1);
        t.setPC(static_cast<uint32_t>(target));
        advancePC = false;
      }
      break;
    }

    case Opcode::Halt: {
      t.setState(nsbaci::types::ThreadState::Terminated);
      advancePC = false;
      break;
    }

    // ============== Concurrency - Semaphores ==============
    case Opcode::Wait: {
      // TODO: Implement semaphore wait
      // Get semaphore address, decrement, block if < 0
      break;
    }

    case Opcode::Signal: {
      // TODO: Implement semaphore signal
      // Get semaphore address, increment, wake waiting thread if any
      break;
    }

    // ============== Concurrency - Process ==============
    case Opcode::Cobegin: {
      // TODO: Mark start of concurrent block
      break;
    }

    case Opcode::Coend: {
      // TODO: Wait for all concurrent threads to finish
      break;
    }

    // ============== I/O Operations ==============
    case Opcode::Write: {
      int32_t value = t.pop();
      result.output = std::to_string(value);
      if (outputCallback) {
        outputCallback(result.output);
      }
      break;
    }

    case Opcode::Writeln: {
      result.output = "\n";
      if (outputCallback) {
        outputCallback(result.output);
      }
      break;
    }

    case Opcode::WriteRawString: {
      std::string str = std::get<std::string>(instr.operand1);
      result.output = str;
      if (outputCallback) {
        outputCallback(result.output);
      }
      break;
    }

    case Opcode::Read: {
      if (!hasInput) {
        // Need input - request it and don't advance PC
        // Thread stays in Running state; controller will pause execution
        waitingForInput = true;
        result.needsInput = true;
        result.inputPrompt = "Enter value: ";
        advancePC = false;
      } else {
        // Input available - parse and push
        try {
          int32_t value = std::stoi(pendingInput);
          t.push(value);
          hasInput = false;
          waitingForInput = false;
        } catch (...) {
          nsbaci::Error err;
          err.basic.severity = nsbaci::types::ErrSeverity::Error;
          err.basic.message = "Invalid input: expected integer";
          err.basic.type = nsbaci::types::ErrType::unknown;
          err.payload = nsbaci::types::RuntimeError{};
          return InterpreterResult(std::move(err));
        }
      }
      break;
    }

    // ============== Default ==============
    default: {
      nsbaci::Error err;
      err.basic.severity = nsbaci::types::ErrSeverity::Error;
      err.basic.message =
          "Unimplemented opcode: " + std::string(opcodeName(instr.opcode));
      err.basic.type = nsbaci::types::ErrType::unknown;
      err.payload = nsbaci::types::RuntimeError{};
      return InterpreterResult(std::move(err));
    }
  }

  if (advancePC) {
    t.advancePC();
  }

  return result;
}

void NsbaciInterpreter::provideInput(const std::string& input) {
  pendingInput = input;
  hasInput = true;
}

bool NsbaciInterpreter::isWaitingForInput() const { return waitingForInput; }

void NsbaciInterpreter::setOutputCallback(OutputCallback callback) {
  outputCallback = std::move(callback);
}

}  // namespace nsbaci::services::runtime
