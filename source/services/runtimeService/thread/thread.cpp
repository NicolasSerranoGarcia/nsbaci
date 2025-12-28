/**
 * @file thread.cc
 * @brief Thread class implementation for nsbaci runtime service.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "thread.h"

namespace nsbaci::services::runtime {

ThreadID Thread::nextThreadId = 0;

ThreadID Thread::getId() const { return id; }

ThreadState Thread::getState() const { return state; }

void Thread::setState(ThreadState newState) { state = newState; }

Priority Thread::getPriority() const { return priority; }

void Thread::setPriority(Priority newPriority) { priority = newPriority; }

}  // namespace nsbaci::services::runtime
