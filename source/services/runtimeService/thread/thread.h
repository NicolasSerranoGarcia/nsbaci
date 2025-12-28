/**
 * @file thread.h
 * @brief Thread class declaration for nsbaci runtime service.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_SERVICES_RUNTIME_THREAD_H
#define NSBACI_SERVICES_RUNTIME_THREAD_H

#include "runtimeTypes.h"

using namespace nsbaci::types;

/**
 * @namespace nsbaci::services::runtime
 * @brief Runtime services namespace for nsbaci.
 */
namespace nsbaci::services::runtime {

/**
 * @class Thread
 * @brief Represents a thread in the runtime service.
 */
class Thread {
 public:
  Thread() : id(nextThreadId++){};
  ~Thread() = default;

  /**
   * @brief Gets the thread ID.
   * @return The unique identifier of this thread.
   */
  ThreadID getId() const;

  /**
   * @brief Gets the current state of the thread.
   * @return The current ThreadState.
   */
  ThreadState getState() const;

  /**
   * @brief Sets the state of the thread.
   * @param newState The new state to set.
   */
  void setState(ThreadState newState);

  /**
   * @brief Gets the priority of the thread.
   * @return The current Priority level.
   */
  Priority getPriority() const;

  /**
   * @brief Sets the priority of the thread.
   * @param newPriority The new priority level to set.
   */
  void setPriority(Priority newPriority);

 private:
  ThreadID id;
  ThreadState state;
  Priority priority;
  // the pc but for this specific thread
  Address threadCounter;

  static ThreadID nextThreadId;

  // friend the scheduler
};

}  // namespace nsbaci::services::runtime

#endif  // NSBACI_SERVICES_RUNTIME_THREAD_H