/**
 * @file controller.cpp
 * @brief Implementation of the Controller class for nsbaci.
 *
 * This file contains the implementation of all Controller methods that
 * coordinate between the user interface and backend services. It handles
 * the complete workflow from file operations through compilation to
 * runtime execution and monitoring.
 *
 * The implementation uses Qt's signal-slot mechanism for asynchronous
 * communication with the UI, and a QTimer-based approach for continuous
 * program execution that maintains UI responsiveness.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "controller.h"

#include "instruction.h"

using namespace nsbaci::types;
using namespace nsbaci::services;

namespace nsbaci {

Controller::Controller(FileService&& f, CompilerService&& c, RuntimeService&& r,
                       DrawingService&& d, QObject* parent)
    : QObject(parent),
      fileService(std::move(f)),
      compilerService(std::move(c)),
      runtimeService(std::move(r)),
      drawingService(std::move(d)) {
  // Set up the run timer for continuous execution
  runTimer = new QTimer(this);
  runTimer->setInterval(1000);  // 10ms between batches for responsive UI
  connect(runTimer, &QTimer::timeout, this, &Controller::runBatch);
}

void Controller::onSaveRequested(File file, Text contents) {
  auto saveRes = fileService.save(contents, file);

  if (!saveRes.ok) {
    auto uiErrors = UIError::fromBackendErrors(saveRes.errors);
    emit saveFailed(std::move(uiErrors));
  } else {
    emit saveSucceeded();
  }
}

void Controller::onOpenRequested(File file) {
  auto openRes = fileService.load(file);

  if (!openRes.ok) {
    auto uiErrors = UIError::fromBackendErrors(openRes.errors);
    emit loadFailed(std::move(uiErrors));
  } else {
    emit loadSucceeded(QString::fromStdString(openRes.contents));
  }
}

void Controller::onCompileRequested(Text contents) {
  auto compileRes = compilerService.compile(contents);

  if (!compileRes.ok) {
    auto uiErrors = UIError::fromBackendErrors(compileRes.errors);
    emit compileFailed(std::move(uiErrors));
  } else {
    programLoaded = true;
    emit compileSucceeded();
  }
}

void Controller::onRunRequested() {
  if (!compilerService.hasProgramReady()) {
    return;
  }

  // Get the compiled program and symbols, load into runtime
  auto instructions = compilerService.takeInstructions();
  auto symbols = compilerService.takeSymbols();
  runtimeService.loadProgram(
      services::runtime::Program(std::move(instructions), std::move(symbols)));

  // Set up output callback to forward to UI
  runtimeService.setOutputCallback([this](const std::string& output) {
    emit outputReceived(QString::fromStdString(output));
  });

  currentProgramName = "Program";  // TODO: Get actual name from file
  programLoaded = true;
  emit runStarted(currentProgramName);
  updateRuntimeDisplay();
}

void Controller::onStepRequested() {
  auto result = runtimeService.step();

  if (!result.ok) {
    auto uiErrors = UIError::fromBackendErrors(result.errors);
    // TODO: Handle runtime errors
  }

  // Handle input requests
  if (result.needsInput) {
    emit inputRequested(QString::fromStdString(result.inputPrompt));
  }
  // Note: output is handled by the outputCallback set in onRunRequested()

  bool running = runtimeService.getState() == RuntimeState::Running;
  bool halted = runtimeService.isHalted();
  emit runtimeStateChanged(running, halted);
  updateRuntimeDisplay();
}

void Controller::onStepThreadRequested(ThreadID threadId) {
  auto result = runtimeService.stepThread(threadId);

  if (!result.ok) {
    auto uiErrors = UIError::fromBackendErrors(result.errors);
    // TODO: Handle runtime errors
  }

  // Handle input requests
  if (result.needsInput) {
    emit inputRequested(QString::fromStdString(result.inputPrompt));
  }
  // Note: output is handled by the outputCallback set in onRunRequested()

  bool running = runtimeService.getState() == RuntimeState::Running;
  bool halted = runtimeService.isHalted();
  emit runtimeStateChanged(running, halted);
  updateRuntimeDisplay();
}

void Controller::onRunContinueRequested() {
  if (runtimeService.isHalted()) {
    return;
  }

  isRunning = true;
  emit runtimeStateChanged(true, false);
  runTimer->start();
}

void Controller::runBatch() {
  // Execute a small batch of steps
  const int BATCH_SIZE = 10;

  for (int i = 0; i < BATCH_SIZE && isRunning; ++i) {
    auto result = runtimeService.step();

    if (!result.ok) {
      // Emit the error message for debugging
      if (!result.errors.empty()) {
        emit outputReceived(QString::fromStdString(
            "Runtime error: " + result.errors[0].basic.message + "\n"));
      }
      isRunning = false;
      runTimer->stop();
      emit runtimeStateChanged(false, false);
      updateRuntimeDisplay();
      return;
    }

    // Handle I/O
    if (result.needsInput) {
      emit inputRequested(QString::fromStdString(result.inputPrompt));
      wasRunningBeforeInput = true;  // Remember we were running
      isRunning = false;
      runTimer->stop();
      emit runtimeStateChanged(false, false);
      updateRuntimeDisplay();
      return;
    }

    if (result.halted) {
      emit outputReceived(QString("Program halted.\n"));
      isRunning = false;
      runTimer->stop();
      emit runtimeStateChanged(false, true);
      updateRuntimeDisplay();
      return;
    }
  }

  // Update display periodically during execution
  updateRuntimeDisplay();
}

void Controller::onPauseRequested() {
  isRunning = false;
  runTimer->stop();
  runtimeService.pause();

  bool halted = runtimeService.isHalted();
  emit runtimeStateChanged(false, halted);
  updateRuntimeDisplay();
}

void Controller::onResetRequested() {
  isRunning = false;
  runTimer->stop();
  runtimeService.reset();

  emit runtimeStateChanged(false, false);
  updateRuntimeDisplay();
}

void Controller::onStopRequested() {
  isRunning = false;
  runTimer->stop();
  runtimeService.reset();
  programLoaded = false;
}

void Controller::onInputProvided(const QString& input) {
  runtimeService.provideInput(input.toStdString());

  // If we were running continuously before input was requested, resume
  if (wasRunningBeforeInput) {
    wasRunningBeforeInput = false;
    onRunContinueRequested();
  } else {
    // Just do a single step
    onStepRequested();
  }
}

void Controller::updateRuntimeDisplay() {
  auto threads = gatherThreadInfo();
  auto variables = gatherVariableInfo();

  emit threadsUpdated(threads);
  emit variablesUpdated(variables);
}

std::vector<nsbaci::ui::ThreadInfo> Controller::gatherThreadInfo() {
  std::vector<nsbaci::ui::ThreadInfo> result;

  const auto& threads = runtimeService.getThreads();
  const auto& program = runtimeService.getProgram();

  for (const auto& thread : threads) {
    nsbaci::ui::ThreadInfo info;
    info.id = thread.getId();
    info.state = thread.getState();
    info.pc = thread.getPC();

    // Get current instruction name
    if (info.pc < program.instructionCount()) {
      const auto& instr =
          program.getInstruction(static_cast<uint32_t>(info.pc));
      info.currentInstruction = QString::fromStdString(
          std::string(nsbaci::compiler::opcodeName(instr.opcode)));
    } else {
      info.currentInstruction = "---";
    }

    result.push_back(info);
  }

  return result;
}

std::vector<nsbaci::ui::VariableInfo> Controller::gatherVariableInfo() {
  std::vector<nsbaci::ui::VariableInfo> result;

  const auto& program = runtimeService.getProgram();
  const auto& symbols = program.symbols();

  for (const auto& [name, info] : symbols) {
    nsbaci::ui::VariableInfo varInfo;
    varInfo.name = QString::fromStdString(name);
    varInfo.type = QString::fromStdString(info.type);
    varInfo.address = info.address;
    varInfo.value = QString::number(program.readMemory(info.address));
    result.push_back(varInfo);
  }

  return result;
}

}  // namespace nsbaci
