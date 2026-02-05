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

void NsbaciScheduler::blockOnSemaphore(uint32_t semaphoreAddr) {
  if (!runningIndex.has_value()) {
    return;
  }

  Thread& current = threads[runningIndex.value()];
  current.setState(nsbaci::types::ThreadState::Blocked);
  semaphoreQueues[semaphoreAddr].push_back(runningIndex.value());
  runningIndex = std::nullopt;
}

size_t NsbaciScheduler::unblockSemaphore(uint32_t semaphoreAddr) {
  auto it = semaphoreQueues.find(semaphoreAddr);
  if (it == semaphoreQueues.end() || it->second.empty()) {
    return 0;
  }

  // Unblock one thread waiting on this semaphore (FIFO order)
  size_t threadIndex = it->second.front();
  it->second.erase(it->second.begin());

  threads[threadIndex].setState(nsbaci::types::ThreadState::Ready);
  readyQueue.push_back(threadIndex);

  // Clean up empty queue
  if (it->second.empty()) {
    semaphoreQueues.erase(it);
  }

  return 1;
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

  // Check if any coend-blocked threads should be unblocked
  checkCoendUnblock();
}

bool NsbaciScheduler::hasThreads() const {
  return runningIndex.has_value() || !readyQueue.empty() || 
         !blockedQueue.empty() || !ioQueue.empty() || !coendQueue.empty();
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
  semaphoreQueues.clear();
  coendQueue.clear();
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

void NsbaciScheduler::blockOnCoend(int32_t expectedThreads) {
  if (!runningIndex.has_value()) {
    return;
  }

  Thread& current = threads[runningIndex.value()];
  current.setState(nsbaci::types::ThreadState::Blocked);
  coendQueue.push_back({runningIndex.value(), expectedThreads});
  runningIndex = std::nullopt;
}

void NsbaciScheduler::checkCoendUnblock() {
  if (coendQueue.empty()) {
    return;
  }

  // Count non-terminated threads (excluding coend-blocked threads)
  size_t activeThreads = 0;
  for (size_t i = 0; i < threads.size(); ++i) {
    auto state = threads[i].getState();
    if (state != nsbaci::types::ThreadState::Terminated) {
      // Check if this thread is coend-blocked
      bool isCoendBlocked = false;
      for (const auto& [idx, _] : coendQueue) {
        if (idx == i) {
          isCoendBlocked = true;
          break;
        }
      }
      if (!isCoendBlocked) {
        activeThreads++;
      }
    }
  }

  // If no active threads (other than coend-blocked), unblock all coend threads
  if (activeThreads == 0) {
    for (const auto& [idx, _] : coendQueue) {
      threads[idx].setState(nsbaci::types::ThreadState::Ready);
      readyQueue.push_back(idx);
    }
    coendQueue.clear();
  }
}

}  // namespace nsbaci::services::runtime
