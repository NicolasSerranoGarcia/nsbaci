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

#include <queue>

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

 private:
  nsbaci::types::ThreadQueue blocked;  ///< Queue of blocked threads
  nsbaci::types::ThreadQueue ready;    ///< Queue of threads ready to run
  nsbaci::types::ThreadQueue io;       ///< Queue of threads waiting for I/O
  Thread running;                      ///< Currently running thread
};

}  // namespace nsbaci::services::runtime

#endif  // NSBACI_SERVICES_RUNTIME_SCHEDULER_H