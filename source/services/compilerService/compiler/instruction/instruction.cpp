/**
 * @file instruction.cpp
 * @brief Instruction implementation for nsbaci compiler.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "instruction.h"

namespace nsbaci::compiler {

const char* opcodeName(Opcode op) {
  switch (op) {
    // Stack/Memory Operations
    case Opcode::LoadValue:
      return "LoadValue";
    case Opcode::LoadAddress:
      return "LoadAddress";
    case Opcode::LoadIndirect:
      return "LoadIndirect";
    case Opcode::LoadBlock:
      return "LoadBlock";
    case Opcode::Store:
      return "Store";
    case Opcode::StoreKeep:
      return "StoreKeep";
    case Opcode::PushLiteral:
      return "PushLiteral";
    case Opcode::Index:
      return "Index";
    case Opcode::CopyBlock:
      return "CopyBlock";
    case Opcode::ValueAt:
      return "ValueAt";
    case Opcode::MarkStack:
      return "MarkStack";
    case Opcode::UpdateDisplay:
      return "UpdateDisplay";

    // Arithmetic Operations
    case Opcode::Add:
      return "Add";
    case Opcode::Sub:
      return "Sub";
    case Opcode::Mult:
      return "Mult";
    case Opcode::Div:
      return "Div";
    case Opcode::Mod:
      return "Mod";
    case Opcode::Negate:
      return "Negate";
    case Opcode::Complement:
      return "Complement";

    // Logical Operations
    case Opcode::And:
      return "And";
    case Opcode::Or:
      return "Or";

    // Comparison Operations
    case Opcode::TestEQ:
      return "TestEQ";
    case Opcode::TestNE:
      return "TestNE";
    case Opcode::TestLT:
      return "TestLT";
    case Opcode::TestLE:
      return "TestLE";
    case Opcode::TestGT:
      return "TestGT";
    case Opcode::TestGE:
      return "TestGE";
    case Opcode::TestEqualKeep:
      return "TestEqualKeep";

    // Control Flow
    case Opcode::Jump:
      return "Jump";
    case Opcode::JumpZero:
      return "JumpZero";
    case Opcode::Call:
      return "Call";
    case Opcode::ShortCall:
      return "ShortCall";
    case Opcode::ShortReturn:
      return "ShortReturn";
    case Opcode::ExitProc:
      return "ExitProc";
    case Opcode::ExitFunction:
      return "ExitFunction";
    case Opcode::Halt:
      return "Halt";

    // Loop Control
    case Opcode::BeginFor:
      return "BeginFor";
    case Opcode::EndFor:
      return "EndFor";

    // Concurrency - Process
    case Opcode::Cobegin:
      return "Cobegin";
    case Opcode::Coend:
      return "Coend";
    case Opcode::Create:
      return "Create";
    case Opcode::Suspend:
      return "Suspend";
    case Opcode::Revive:
      return "Revive";
    case Opcode::WhichProc:
      return "WhichProc";

    // Concurrency - Semaphores
    case Opcode::Wait:
      return "Wait";
    case Opcode::Signal:
      return "Signal";
    case Opcode::StoreSemaphore:
      return "StoreSemaphore";

    // Concurrency - Monitors
    case Opcode::EnterMonitor:
      return "EnterMonitor";
    case Opcode::ExitMonitor:
      return "ExitMonitor";
    case Opcode::CallMonitorInit:
      return "CallMonitorInit";
    case Opcode::ReturnMonitorInit:
      return "ReturnMonitorInit";
    case Opcode::WaitCondition:
      return "WaitCondition";
    case Opcode::SignalCondition:
      return "SignalCondition";
    case Opcode::Empty:
      return "Empty";

    // I/O Operations
    case Opcode::Read:
      return "Read";
    case Opcode::Readln:
      return "Readln";
    case Opcode::Write:
      return "Write";
    case Opcode::Writeln:
      return "Writeln";
    case Opcode::WriteString:
      return "WriteString";
    case Opcode::WriteRawString:
      return "WriteRawString";
    case Opcode::EolEof:
      return "EolEof";
    case Opcode::Sprintf:
      return "Sprintf";
    case Opcode::Sscanf:
      return "Sscanf";

    // String Operations
    case Opcode::CopyString:
      return "CopyString";
    case Opcode::CopyRawString:
      return "CopyRawString";
    case Opcode::ConcatString:
      return "ConcatString";
    case Opcode::ConcatRawString:
      return "ConcatRawString";
    case Opcode::CompareString:
      return "CompareString";
    case Opcode::CompareRawString:
      return "CompareRawString";
    case Opcode::LengthString:
      return "LengthString";

    // Graphics Operations
    case Opcode::MoveTo:
      return "MoveTo";
    case Opcode::MoveBy:
      return "MoveBy";
    case Opcode::ChangeColor:
      return "ChangeColor";
    case Opcode::MakeVisible:
      return "MakeVisible";
    case Opcode::Remove:
      return "Remove";

    // Miscellaneous
    case Opcode::Random:
      return "Random";
    case Opcode::Test:
      return "Test";

    case Opcode::_Count:
      return "_Count";
  }
  return "Unknown";
}

}  // namespace nsbaci::compiler
