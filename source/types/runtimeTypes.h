/**
 * @file runtimeTypes.h
 * @brief Type definitions for runtime-related operations.
 *
 * This header provides type aliases used by the RuntimeService
 * and other components that work with runtime execution.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_TYPES_RUNTIMETYPES_H
#define NSBACI_TYPES_RUNTIMETYPES_H

/**
 * @namespace nsbaci::types
 * @brief Type definitions namespace for nsbaci.
 */
namespace nsbaci::types {

using ThreadID = unsigned long long int;

using Priority = unsigned long int;

enum class ThreadState {
  Ready,      ///< Thread is ready to run
  Running,    ///< Thread is currently executing
  Blocked,    ///< Thread is blocked (e.g., waiting on semaphore)
  Waiting,    ///< Thread is waiting for I/O
  IO,         ///< Thread is performing I/O
  Terminated  ///< Thread has finished execution
};

}  // namespace nsbaci::types

#endif  // NSBACI_TYPES_RUNTIMETYPES_H
