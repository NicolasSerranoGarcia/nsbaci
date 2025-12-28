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

#include <stack>

/**
 * @namespace nsbaci::types
 * @brief Type definitions namespace for nsbaci.
 */
namespace nsbaci::types {
// just in case I want to implement my own stack or smth in the future
template <typename Type>
using Stack = std::stack<Type>;

using ThreadID = unsigned long long int;

using Priority = unsigned long int;

enum class ThreadState { Waiting, Ready, Running, Blocked, IO };

using Address = unsigned long long int;

}  // namespace nsbaci::types

#endif  // NSBACI_TYPES_RUNTIMETYPES_H
