/**
 * @file nsbaciInterpreter.h
 * @brief NsbaciInterpreter class declaration for nsbaci runtime service.
 *
 * This module provides the concrete BACI-specific implementation of the
 * Interpreter interface for executing BACI instructions.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_SERVICES_RUNTIME_NSBACI_INTERPRETER_H
#define NSBACI_SERVICES_RUNTIME_NSBACI_INTERPRETER_H

#include "interpreter.h"

/**
 * @namespace nsbaci::services::runtime
 * @brief Runtime services namespace for nsbaci.
 */
namespace nsbaci::services::runtime {

/**
 * @class NsbaciInterpreter
 * @brief BACI-specific implementation of the Interpreter.
 *
 * NsbaciInterpreter executes BACI p-code instructions, managing
 * the execution state and interaction with the program context.
 */
class NsbaciInterpreter final : public Interpreter {
 public:
  NsbaciInterpreter() = default;
  ~NsbaciInterpreter() override = default;

  /**
   * @brief Executes the current instruction for the given thread with the
   * program context.
   * @param t The thread whose instruction should be executed.
   * @param program The program context in which to execute the instruction.
   * @return InterpreterResult indicating success or any errors encountered.
   */
  InterpreterResult executeInstruction(Thread& t, Program& program) override;

  void provideInput(const std::string& input) override;
  bool isWaitingForInput() const override;
  void setOutputCallback(OutputCallback callback) override;
  void setDrawingCallback(DrawingCallback callback) override;
  void reset() override;

 private:
  OutputCallback outputCallback;
  DrawingCallback drawingCallback;
  bool waitingForInput = false;
  std::string pendingInput;
  bool hasInput = false;

  // Drawing state
  uint8_t currentR = 0, currentG = 0, currentB = 0, currentA = 255;
  int32_t currentLineWidth = 1;
};

}  // namespace nsbaci::services::runtime

#endif  // NSBACI_SERVICES_RUNTIME_NSBACI_INTERPRETER_H