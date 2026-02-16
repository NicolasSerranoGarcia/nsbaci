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

    // ============== Drawing Operations ==============
    case Opcode::DrawClear: {
      if (drawingCallback) {
        drawingCallback(nsbaci::types::DrawCommand::clear());
      }
      break;
    }

    case Opcode::DrawRefresh: {
      if (drawingCallback) {
        drawingCallback(nsbaci::types::DrawCommand::refresh());
      }
      break;
    }

    case Opcode::DrawSetColor: {
      // Stack: r, g, b (popped in reverse order)
      int32_t b = t.pop();
      int32_t g = t.pop();
      int32_t r = t.pop();
      currentR = static_cast<uint8_t>(r & 0xFF);
      currentG = static_cast<uint8_t>(g & 0xFF);
      currentB = static_cast<uint8_t>(b & 0xFF);
      currentA = 255;
      if (drawingCallback) {
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::setColor(color));
      }
      break;
    }

    case Opcode::DrawSetColorAlpha: {
      // Stack: r, g, b, a (popped in reverse order)
      int32_t a = t.pop();
      int32_t b = t.pop();
      int32_t g = t.pop();
      int32_t r = t.pop();
      currentR = static_cast<uint8_t>(r & 0xFF);
      currentG = static_cast<uint8_t>(g & 0xFF);
      currentB = static_cast<uint8_t>(b & 0xFF);
      currentA = static_cast<uint8_t>(a & 0xFF);
      if (drawingCallback) {
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::setColor(color));
      }
      break;
    }

    case Opcode::DrawSetLineWidth: {
      currentLineWidth = t.pop();
      if (drawingCallback) {
        drawingCallback(nsbaci::types::DrawCommand::setLineWidth(currentLineWidth));
      }
      break;
    }

    case Opcode::DrawSetPosition: {
      // Stack: x, y
      int32_t y = t.pop();
      int32_t x = t.pop();
      if (drawingCallback) {
        drawingCallback(nsbaci::types::DrawCommand::setPosition(
            nsbaci::types::Point(x, y)));
      }
      break;
    }

    case Opcode::DrawCircle: {
      // Stack: x, y, radius (outline only)
      int32_t radius = t.pop();
      int32_t y = t.pop();
      int32_t x = t.pop();
      if (drawingCallback) {
        nsbaci::types::Circle circle(
            nsbaci::types::Point(x, y), radius, false);
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::drawShape(circle, color));
      }
      break;
    }

    case Opcode::FillCircle: {
      // Stack: x, y, radius (filled)
      int32_t radius = t.pop();
      int32_t y = t.pop();
      int32_t x = t.pop();
      if (drawingCallback) {
        nsbaci::types::Circle circle(
            nsbaci::types::Point(x, y), radius, true);
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::drawShape(circle, color));
      }
      break;
    }

    case Opcode::DrawRectangle: {
      // Stack: x, y, width, height (outline only)
      int32_t height = t.pop();
      int32_t width = t.pop();
      int32_t y = t.pop();
      int32_t x = t.pop();
      if (drawingCallback) {
        nsbaci::types::Rectangle rect(x, y, width, height, false);
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::drawShape(rect, color));
      }
      break;
    }

    case Opcode::FillRectangle: {
      // Stack: x, y, width, height (filled)
      int32_t height = t.pop();
      int32_t width = t.pop();
      int32_t y = t.pop();
      int32_t x = t.pop();
      if (drawingCallback) {
        nsbaci::types::Rectangle rect(x, y, width, height, true);
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::drawShape(rect, color));
      }
      break;
    }

    case Opcode::DrawTriangle: {
      // Stack: x1, y1, x2, y2, x3, y3 (outline only)
      int32_t y3 = t.pop();
      int32_t x3 = t.pop();
      int32_t y2 = t.pop();
      int32_t x2 = t.pop();
      int32_t y1 = t.pop();
      int32_t x1 = t.pop();
      if (drawingCallback) {
        nsbaci::types::Triangle tri(
            nsbaci::types::Point(x1, y1),
            nsbaci::types::Point(x2, y2),
            nsbaci::types::Point(x3, y3), false);
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::drawShape(tri, color));
      }
      break;
    }

    case Opcode::FillTriangle: {
      // Stack: x1, y1, x2, y2, x3, y3 (filled)
      int32_t y3 = t.pop();
      int32_t x3 = t.pop();
      int32_t y2 = t.pop();
      int32_t x2 = t.pop();
      int32_t y1 = t.pop();
      int32_t x1 = t.pop();
      if (drawingCallback) {
        nsbaci::types::Triangle tri(
            nsbaci::types::Point(x1, y1),
            nsbaci::types::Point(x2, y2),
            nsbaci::types::Point(x3, y3), true);
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::drawShape(tri, color));
      }
      break;
    }

    case Opcode::DrawLine: {
      // Stack: x1, y1, x2, y2
      int32_t y2 = t.pop();
      int32_t x2 = t.pop();
      int32_t y1 = t.pop();
      int32_t x1 = t.pop();
      if (drawingCallback) {
        nsbaci::types::Line line(
            nsbaci::types::Point(x1, y1),
            nsbaci::types::Point(x2, y2), currentLineWidth);
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::drawShape(line, color));
      }
      break;
    }

    case Opcode::DrawEllipse: {
      // Stack: x, y, radiusX, radiusY (outline only)
      int32_t radiusY = t.pop();
      int32_t radiusX = t.pop();
      int32_t y = t.pop();
      int32_t x = t.pop();
      if (drawingCallback) {
        nsbaci::types::Ellipse ellipse(
            nsbaci::types::Point(x, y), radiusX, radiusY, false);
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::drawShape(ellipse, color));
      }
      break;
    }

    case Opcode::FillEllipse: {
      // Stack: x, y, radiusX, radiusY (filled)
      int32_t radiusY = t.pop();
      int32_t radiusX = t.pop();
      int32_t y = t.pop();
      int32_t x = t.pop();
      if (drawingCallback) {
        nsbaci::types::Ellipse ellipse(
            nsbaci::types::Point(x, y), radiusX, radiusY, true);
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::drawShape(ellipse, color));
      }
      break;
    }

    case Opcode::DrawPixel: {
      // Stack: x, y
      int32_t y = t.pop();
      int32_t x = t.pop();
      if (drawingCallback) {
        nsbaci::types::Pixel pixel(x, y);
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::drawShape(pixel, color));
      }
      break;
    }

    case Opcode::DrawText: {
      // Stack: x, y, fontSize; operand1 = text string
      int32_t fontSize = t.pop();
      int32_t y = t.pop();
      int32_t x = t.pop();
      std::string text = std::get<std::string>(instr.operand1);
      if (drawingCallback) {
        nsbaci::types::DrawText drawText(
            nsbaci::types::Point(x, y), text, fontSize);
        nsbaci::types::Color color(currentR, currentG, currentB, currentA);
        drawingCallback(nsbaci::types::DrawCommand::drawShape(drawText, color));
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

void NsbaciInterpreter::setDrawingCallback(DrawingCallback callback) {
  drawingCallback = std::move(callback);
}

void NsbaciInterpreter::reset() {
  waitingForInput = false;
  pendingInput.clear();
  hasInput = false;
  currentR = 0;
  currentG = 0;
  currentB = 0;
  currentA = 255;
  currentLineWidth = 1;
}

}  // namespace nsbaci::services::runtime
