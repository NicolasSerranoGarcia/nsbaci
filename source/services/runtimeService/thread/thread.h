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

#include <queue>

#include "runtimeTypes.h"

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
  nsbaci::types::ThreadID getId() const;

  /**
   * @brief Gets the current state of the thread.
   * @return The current ThreadState.
   */
  nsbaci::types::ThreadState getState() const;

  /**
   * @brief Sets the state of the thread.
   * @param newState The new state to set.
   */
  void setState(nsbaci::types::ThreadState newState);

  /**
   * @brief Gets the priority of the thread.
   * @return The current Priority level.
   */
  nsbaci::types::Priority getPriority() const;

  /**
   * @brief Sets the priority of the thread.
   * @param newPriority The new priority level to set.
   */
  void setPriority(nsbaci::types::Priority newPriority);

 private:
  nsbaci::types::ThreadID id;
  nsbaci::types::ThreadState state;
  nsbaci::types::Priority priority;
  // the pc but for this specific thread (thread counter)
  nsbaci::types::Address tc;

  static nsbaci::types::ThreadID nextThreadId;

  // friend the scheduler
};

}  // namespace nsbaci::services::runtime

/**
 * @namespace nsbaci::types
 * @brief Type definitions namespace for nsbaci.
 */
namespace nsbaci::types {

/// @brief Queue of threads for scheduler operations
using ThreadQueue = std::queue<nsbaci::services::runtime::Thread>;

}  // namespace nsbaci::types

#endif  // NSBACI_SERVICES_RUNTIME_THREAD_H