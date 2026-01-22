/**
 * @file thread.cpp
 * @brief Thread class implementation for nsbaci runtime service.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "thread.h"

#include <stdexcept>

using namespace nsbaci::types;

namespace nsbaci::services::runtime {

ThreadID Thread::nextThreadId = 0;

ThreadID Thread::getId() const { return id; }

ThreadState Thread::getState() const { return state; }

void Thread::setState(ThreadState newState) { state = newState; }

Priority Thread::getPriority() const { return priority; }

void Thread::setPriority(Priority newPriority) { priority = newPriority; }

void Thread::push(int32_t value) {
  stack.push_back(value);
  ++sp;
}

int32_t Thread::pop() {
  if (stack.empty()) {
    throw std::runtime_error("Stack underflow");
  }
  int32_t value = stack.back();
  stack.pop_back();
  --sp;
  return value;
}

int32_t Thread::top() const {
  if (stack.empty()) {
    throw std::runtime_error("Stack is empty");
  }
  return stack.back();
}

}  // namespace nsbaci::services::runtime
