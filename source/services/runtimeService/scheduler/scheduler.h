/**
 * @file scheduler.h
 * @brief Scheduler class declaration for nsbaci runtime service.
 *
 * This module defines the thread scheduler responsible for managing
 * thread execution order and state transitions.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_SERVICES_RUNTIME_SCHEDULER_H
#define NSBACI_SERVICES_RUNTIME_SCHEDULER_H

#include <optional>
#include <vector>

#include "thread.h"

/**
 * @namespace nsbaci::services::runtime
 * @brief Runtime services namespace for nsbaci.
 */
namespace nsbaci::services::runtime {

/**
 * @class Scheduler
 * @brief Manages thread scheduling and state transitions.
 *
 * The Scheduler is responsible for determining which thread runs next,
 * managing thread queues for different states, and handling thread
 * state transitions.
 */
class Scheduler {
 public:
  Scheduler() = default;
  virtual ~Scheduler() = default;

  /**
   * @brief Pick the next thread to run.
   * @return Pointer to the next thread, or nullptr if no threads are ready.
   */
  virtual Thread* pickNext() = 0;

  /**
   * @brief Add a new thread to the scheduler.
   * @param thread The thread to add.
   */
  virtual void addThread(Thread thread) = 0;

  /**
   * @brief Block the currently running thread.
   */
  virtual void blockCurrent() = 0;

  /**
   * @brief Move a thread from blocked to ready state.
   * @param threadId The ID of the thread to unblock.
   */
  virtual void unblock(nsbaci::types::ThreadID threadId) = 0;

  /**
   * @brief Yield the current thread (move to back of ready queue).
   */
  virtual void yield() = 0;

  /**
   * @brief Terminate the currently running thread.
   */
  virtual void terminateCurrent() = 0;

  /**
   * @brief Check if there are any threads left to run.
   * @return True if there are threads in any queue.
   */
  virtual bool hasThreads() const = 0;

  /**
   * @brief Get the currently running thread.
   * @return Pointer to current thread, or nullptr if none.
   */
  virtual Thread* current() = 0;

  /**
   * @brief Clear all threads and reset scheduler state.
   */
  virtual void clear() = 0;

  /**
   * @brief Move all I/O waiting threads back to ready queue.
   * Called when input becomes available.
   */
  virtual void unblockIO() = 0;

  /**
   * @brief Get all threads managed by the scheduler.
   * @return Const reference to the threads vector.
   */
  virtual const std::vector<Thread>& getThreads() const = 0;

 protected:
  std::vector<Thread> threads;         ///< All threads owned by scheduler
  std::vector<size_t> readyQueue;      ///< Indices of ready threads
  std::vector<size_t> blockedQueue;    ///< Indices of blocked threads
  std::vector<size_t> ioQueue;         ///< Indices of I/O waiting threads
  std::optional<size_t> runningIndex;  ///< Index of currently running thread
};

}  // namespace nsbaci::services::runtime

#endif  // NSBACI_SERVICES_RUNTIME_SCHEDULER_H