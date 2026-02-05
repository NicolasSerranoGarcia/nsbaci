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

    case Opcode::StoreIndirect: {
      // Stack: [value, address] -> pop address, pop value, store value at address
      uint32_t addr = static_cast<uint32_t>(t.pop());
      int32_t value = t.pop();
      if (addr >= program.memory().size()) {
        program.memory().resize(addr + 1, 0);
      }
      program.memory()[addr] = value;
      break;
    }

    case Opcode::Swap: {
      // Swap top two stack elements
      int32_t a = t.pop();
      int32_t b = t.pop();
      t.push(a);
      t.push(b);
      break;
    }

    case Opcode::RotateDown3: {
      // Rotate top 3: [a,b,c] (c on top) -> [b,c,a]
      int32_t c = t.pop();
      int32_t b = t.pop();
      int32_t a = t.pop();
      t.push(b);
      t.push(c);
      t.push(a);
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

    case Opcode::Call: {
      // Call function: save return address and jump to function
      int32_t target = std::get<int32_t>(instr.operand1);
      t.pushReturnAddress(t.getPC() + 1);  // Return to instruction after Call
      t.setPC(static_cast<uint32_t>(target));
      advancePC = false;
      break;
    }

    case Opcode::ShortCall: {
      // Short call without display update
      int32_t target = std::get<int32_t>(instr.operand1);
      t.pushReturnAddress(t.getPC() + 1);
      t.setPC(static_cast<uint32_t>(target));
      advancePC = false;
      break;
    }

    case Opcode::ShortReturn: {
      // Return from function (no return value)
      if (t.callStackEmpty()) {
        // No return address - terminate thread
        t.setState(nsbaci::types::ThreadState::Terminated);
        advancePC = false;
      } else {
        uint32_t returnAddr = t.popReturnAddress();
        t.setPC(returnAddr);
        advancePC = false;
      }
      break;
    }

    case Opcode::ExitFunction: {
      // Return from function with return value on stack
      // The return value is already on the stack, just return
      if (t.callStackEmpty()) {
        t.setState(nsbaci::types::ThreadState::Terminated);
        advancePC = false;
      } else {
        uint32_t returnAddr = t.popReturnAddress();
        t.setPC(returnAddr);
        advancePC = false;
      }
      break;
    }

    case Opcode::ExitProc: {
      // Exit procedure (same as ShortReturn for now)
      if (t.callStackEmpty()) {
        t.setState(nsbaci::types::ThreadState::Terminated);
        advancePC = false;
      } else {
        uint32_t returnAddr = t.popReturnAddress();
        t.setPC(returnAddr);
        advancePC = false;
      }
      break;
    }

    case Opcode::EnterFrame: {
      // Save local variable values for this frame
      // operand1 = start address, operand2 = count
      uint32_t startAddr = std::get<uint32_t>(instr.operand1);
      int32_t count = std::get<int32_t>(instr.operand2);
      std::vector<int32_t> savedLocals;
      for (int32_t i = 0; i < count; i++) {
        uint32_t addr = startAddr + i;
        if (addr < program.memory().size()) {
          savedLocals.push_back(program.memory()[addr]);
        } else {
          savedLocals.push_back(0);
        }
      }
      t.pushFrame(savedLocals);
      break;
    }

    case Opcode::LeaveFrame: {
      // Restore local variable values from saved frame
      // operand1 = start address, operand2 = count
      uint32_t startAddr = std::get<uint32_t>(instr.operand1);
      int32_t count = std::get<int32_t>(instr.operand2);
      auto savedLocals = t.popFrame();
      for (size_t i = 0; i < savedLocals.size() && static_cast<int32_t>(i) < count; i++) {
        uint32_t addr = startAddr + i;
        if (addr >= program.memory().size()) {
          program.memory().resize(addr + 1, 0);
        }
        program.memory()[addr] = savedLocals[i];
      }
      break;
    }

    // ============== Concurrency - Semaphores ==============
    case Opcode::StoreSemaphore: {
      // Initialize semaphore: address in operand1, value on stack
      uint32_t addr = std::get<uint32_t>(instr.operand1);
      int32_t value = t.pop();
      if (addr >= program.memory().size()) {
        program.memory().resize(addr + 1, 0);
      }
      program.memory()[addr] = value;
      break;
    }

    case Opcode::Wait: {
      // Semaphore wait (P operation)
      // Address is on the stack
      uint32_t addr = static_cast<uint32_t>(t.pop());
      if (addr >= program.memory().size()) {
        program.memory().resize(addr + 1, 0);
      }

      int32_t semValue = program.memory()[addr];
      if (semValue > 0) {
        // Semaphore available, decrement and continue
        program.memory()[addr] = semValue - 1;
      } else {
        // Semaphore not available, block the thread
        result.shouldBlock = true;
        result.blockingSemaphore = addr;
        // Don't advance PC - will retry when unblocked
        advancePC = false;
        // Push address back for retry
        t.push(static_cast<int32_t>(addr));
      }
      break;
    }

    case Opcode::Signal: {
      // Semaphore signal (V operation)
      // Address is on the stack
      uint32_t addr = static_cast<uint32_t>(t.pop());
      if (addr >= program.memory().size()) {
        program.memory().resize(addr + 1, 0);
      }

      // Increment semaphore
      program.memory()[addr]++;

      // Signal to runtime that a thread may be unblocked
      result.signalSemaphore = true;
      result.signaledSemaphore = addr;
      break;
    }

    // ============== Concurrency - Process ==============
    case Opcode::Cobegin: {
      // No longer used - cobegin is now just a marker that gets optimized out
      // The new code generation uses Jump + Create instructions
      break;
    }

    case Opcode::Coend: {
      // Wait for all concurrent threads to finish
      // operand1 contains the expected number of threads to wait for
      // Runtime service handles the synchronization
      result.coendWait = true;
      result.expectedThreadCount = std::get<int32_t>(instr.operand1);
      break;
    }

    case Opcode::Create: {
      // Create a new thread starting at the address in operand1
      result.createThread = true;
      result.newThreadPC = static_cast<uint32_t>(std::get<int32_t>(instr.operand1));
      break;
    }

    case Opcode::ThreadEnd: {
      // Terminate the current thread
      t.setState(nsbaci::types::ThreadState::Terminated);
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

void NsbaciInterpreter::reset() {
  waitingForInput = false;
  pendingInput.clear();
  hasInput = false;
}

}  // namespace nsbaci::services::runtime
