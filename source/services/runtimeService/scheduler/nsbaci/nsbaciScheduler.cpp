/**
 * @file nsbaciScheduler.cpp
 * @brief NsbaciScheduler class implementation for nsbaci runtime service.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "nsbaciScheduler.h"

#include <random>

namespace nsbaci::services::runtime {

Thread* NsbaciScheduler::pickNext() {
  // If there's a running thread, handle its state
  if (runningIndex.has_value()) {
    Thread& current = threads[runningIndex.value()];
    auto currentState = current.getState();

    if (currentState == nsbaci::types::ThreadState::Running) {
      // Put back in ready queue
      current.setState(nsbaci::types::ThreadState::Ready);
      readyQueue.push_back(runningIndex.value());
    } else if (currentState == nsbaci::types::ThreadState::IO) {
      // Thread is waiting for I/O - put in IO queue
      ioQueue.push_back(runningIndex.value());
    }
    // Other states (Blocked, Terminated) are handled elsewhere
    runningIndex = std::nullopt;
  }

  // No threads ready
  if (readyQueue.empty()) {
    return nullptr;
  }

  // BACI uses random selection to simulate non-determinism
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> dist(0, readyQueue.size() - 1);

  size_t randomIdx = dist(gen);
  size_t nextIndex = readyQueue[randomIdx];

  // Remove selected element by swapping with last and popping
  readyQueue[randomIdx] = readyQueue.back();
  readyQueue.pop_back();

  runningIndex = nextIndex;
  threads[nextIndex].setState(nsbaci::types::ThreadState::Running);

  return &threads[nextIndex];
}

void NsbaciScheduler::addThread(Thread thread) {
  thread.setState(nsbaci::types::ThreadState::Ready);
  size_t index = threads.size();
  threads.push_back(std::move(thread));
  readyQueue.push_back(index);
}

void NsbaciScheduler::blockCurrent() {
  if (!runningIndex.has_value()) {
    return;
  }

  Thread& current = threads[runningIndex.value()];
  current.setState(nsbaci::types::ThreadState::Blocked);
  blockedQueue.push_back(runningIndex.value());
  runningIndex = std::nullopt;
}

void NsbaciScheduler::unblock(nsbaci::types::ThreadID threadId) {
  // Search in blocked queue and move to ready
  for (auto it = blockedQueue.begin(); it != blockedQueue.end(); ++it) {
    if (threads[*it].getId() == threadId) {
      threads[*it].setState(nsbaci::types::ThreadState::Ready);
      readyQueue.push_back(*it);
      blockedQueue.erase(it);
      return;
    }
  }
}

void NsbaciScheduler::yield() {
  if (!runningIndex.has_value()) {
    return;
  }

  Thread& current = threads[runningIndex.value()];
  current.setState(nsbaci::types::ThreadState::Ready);
  readyQueue.push_back(runningIndex.value());
  runningIndex = std::nullopt;
}

void NsbaciScheduler::terminateCurrent() {
  if (!runningIndex.has_value()) {
    return;
  }

  // Mark thread as terminated
  threads[runningIndex.value()].setState(
      nsbaci::types::ThreadState::Terminated);
  runningIndex = std::nullopt;
}

bool NsbaciScheduler::hasThreads() const {
  return runningIndex.has_value() || !readyQueue.empty();
}

Thread* NsbaciScheduler::current() {
  if (!runningIndex.has_value()) {
    return nullptr;
  }
  return &threads[runningIndex.value()];
}

void NsbaciScheduler::clear() {
  threads.clear();
  readyQueue.clear();
  blockedQueue.clear();
  ioQueue.clear();
  runningIndex = std::nullopt;
}

void NsbaciScheduler::unblockIO() {
  // Move all I/O waiting threads back to ready queue
  for (size_t idx : ioQueue) {
    threads[idx].setState(nsbaci::types::ThreadState::Ready);
    readyQueue.push_back(idx);
  }
  ioQueue.clear();
}

const std::vector<Thread>& NsbaciScheduler::getThreads() const {
  return threads;
}

std::optional<size_t> NsbaciScheduler::findThreadIndex(
    nsbaci::types::ThreadID threadId) const {
  for (size_t i = 0; i < threads.size(); ++i) {
    if (threads[i].getId() == threadId) {
      return i;
    }
  }
  return std::nullopt;
}

}  // namespace nsbaci::services::runtime
