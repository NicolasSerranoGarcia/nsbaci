/**
 * @file nsbaciScheduler.h
 * @brief NsbaciScheduler class declaration for nsbaci runtime service.
 *
 * This module provides the concrete BACI-specific implementation of the
 * Scheduler interface for managing thread scheduling in the nsbaci runtime.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_SERVICES_RUNTIME_NSBACI_SCHEDULER_H
#define NSBACI_SERVICES_RUNTIME_NSBACI_SCHEDULER_H

#include "scheduler.h"

/**
 * @namespace nsbaci::services::runtime
 * @brief Runtime services namespace for nsbaci.
 */
namespace nsbaci::services::runtime {

/**
 * @class NsbaciScheduler
 * @brief BACI-specific implementation of the Scheduler.
 *
 * NsbaciScheduler implements a round-robin scheduling algorithm
 * with support for blocked, ready, running, and I/O waiting states.
 * Threads are selected randomly from the ready queue to simulate
 * non-deterministic concurrent execution.
 */
class NsbaciScheduler final : public Scheduler {
 public:
  NsbaciScheduler() = default;
  ~NsbaciScheduler() override = default;

  Thread* pickNext() override;
  void addThread(Thread thread) override;
  void blockCurrent() override;
  void unblock(nsbaci::types::ThreadID threadId) override;
  void yield() override;
  void terminateCurrent() override;
  bool hasThreads() const override;
  Thread* current() override;
  void clear() override;
  void unblockIO() override;
  const std::vector<Thread>& getThreads() const override;

 private:
  /**
   * @brief Find thread index by ID.
   * @param threadId The thread ID to search for.
   * @return Index of the thread, or nullopt if not found.
   */
  std::optional<size_t> findThreadIndex(nsbaci::types::ThreadID threadId) const;
};

}  // namespace nsbaci::services::runtime

#endif  // NSBACI_SERVICES_RUNTIME_NSBACI_SCHEDULER_H
