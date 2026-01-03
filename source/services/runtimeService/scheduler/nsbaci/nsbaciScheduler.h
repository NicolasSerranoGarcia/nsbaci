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
 * NsbaciScheduler implements the scheduling algorithm used by the
 * BACI concurrent programming environment, providing thread management
 * with support for blocked, ready, running, and I/O waiting states.
 */
class NsbaciScheduler final : public Scheduler {
 public:
    NsbaciScheduler() = default;
    ~NsbaciScheduler() override = default;

    // TODO: Override virtual methods from Scheduler when defined

 private:
    nsbaci::types::ThreadQueue blocked;  ///< Queue of blocked threads
    nsbaci::types::ThreadQueue ready;    ///< Queue of threads ready to run
    nsbaci::types::ThreadQueue io;       ///< Queue of threads waiting for I/O
    Thread running;                      ///< Currently running thread
};

}  // namespace nsbaci::services::runtime

#endif  // NSBACI_SERVICES_RUNTIME_NSBACI_SCHEDULER_H
