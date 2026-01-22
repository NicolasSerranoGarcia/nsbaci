/**
 * @file runtimeService.cpp
 * @brief Implementation unit for the RuntimeService.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "runtimeService.h"

namespace nsbaci::services {

RuntimeService::RuntimeService(std::unique_ptr<runtime::Interpreter> i,
                               std::unique_ptr<runtime::Scheduler> s)
    : interpreter(std::move(i)),
      scheduler(std::move(s)),
      state(RuntimeState::Idle) {}

void RuntimeService::loadProgram(runtime::Program&& p) {
  program = std::move(p);
  reset();
}

void RuntimeService::reset() {
  if (scheduler) {
    // Clear all existing threads
    scheduler->clear();

    // Create main thread and add to scheduler
    runtime::Thread mainThread;
    mainThread.setPC(0);  // Start at instruction 0
    scheduler->addThread(std::move(mainThread));
  }
  state = RuntimeState::Paused;
}

RuntimeResult RuntimeService::step() {
  RuntimeResult result;

  if (state == RuntimeState::Halted) {
    result.halted = true;
    return result;
  }

  if (!scheduler || !interpreter) {
    nsbaci::Error err;
    err.basic.severity = nsbaci::types::ErrSeverity::Error;
    err.basic.message = "Runtime not properly initialized";
    err.basic.type = nsbaci::types::ErrType::unknown;
    err.payload = nsbaci::types::RuntimeError{};
    return RuntimeResult(std::move(err));
  }

  // Pick next thread to run
  runtime::Thread* thread = scheduler->pickNext();
  if (!thread) {
    // No threads left - program halted
    state = RuntimeState::Halted;
    result.halted = true;
    return result;
  }

  // Execute one instruction
  InterpreterResult interpResult =
      interpreter->executeInstruction(*thread, program);

  if (!interpResult.ok) {
    result.ok = false;
    result.errors = std::move(interpResult.errors);
    state = RuntimeState::Paused;
    return result;
  }

  // Propagate I/O info
  result.needsInput = interpResult.needsInput;
  result.inputPrompt = std::move(interpResult.inputPrompt);
  result.output = std::move(interpResult.output);

  // Check if thread terminated
  if (thread->getState() == nsbaci::types::ThreadState::Terminated) {
    // Thread finished execution
    if (!scheduler->hasThreads()) {
      state = RuntimeState::Halted;
      result.halted = true;
    }
  }

  return result;
}

RuntimeResult RuntimeService::stepThread(nsbaci::types::ThreadID threadId) {
  // TODO: Implement stepping a specific thread
  // For now, just do a regular step
  return step();
}

RuntimeResult RuntimeService::run(size_t maxSteps) {
  RuntimeResult result;
  state = RuntimeState::Running;

  size_t steps = 0;
  while (state == RuntimeState::Running) {
    result = step();

    if (!result.ok || result.halted) {
      break;
    }

    ++steps;
    if (maxSteps > 0 && steps >= maxSteps) {
      state = RuntimeState::Paused;
      break;
    }
  }

  return result;
}

void RuntimeService::pause() {
  if (state == RuntimeState::Running) {
    state = RuntimeState::Paused;
  }
}

RuntimeState RuntimeService::getState() const { return state; }

bool RuntimeService::isHalted() const { return state == RuntimeState::Halted; }

size_t RuntimeService::threadCount() const {
  if (!scheduler) {
    return 0;
  }
  return scheduler->getThreads().size();
}

const std::vector<runtime::Thread>& RuntimeService::getThreads() const {
  static const std::vector<runtime::Thread> empty;
  if (!scheduler) {
    return empty;
  }
  return scheduler->getThreads();
}

const runtime::Program& RuntimeService::getProgram() const { return program; }

void RuntimeService::provideInput(const std::string& input) {
  if (interpreter) {
    interpreter->provideInput(input);
  }
  // Note: Thread stays in Running state during I/O wait, so no need to unblock
}

bool RuntimeService::isWaitingForInput() const {
  if (!interpreter) {
    return false;
  }
  return interpreter->isWaitingForInput();
}

void RuntimeService::setOutputCallback(runtime::OutputCallback callback) {
  if (interpreter) {
    interpreter->setOutputCallback(std::move(callback));
  }
}

}  // namespace nsbaci::services
