/**
 * @file errorTypes.h
 * @brief Type definitions for error-related structures.
 *
 * This header provides type aliases and enums used by the Error
 * module and other components that handle errors.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_TYPES_ERRORTYPES_H
#define NSBACI_TYPES_ERRORTYPES_H

#include <variant>

/**
 * @namespace nsbaci::types
 * @brief Type definitions namespace for nsbaci.
 */
namespace nsbaci::types {

/**
 * @enum ErrSeverity
 * @brief Severity levels for errors.
 */
enum class ErrSeverity { Warning, Error, Fatal };

/**
 * @struct ErrorBase
 * @brief Base structure containing common error properties.
 */
struct ErrorBase {
  ErrSeverity severity;
};

/**
 * @struct CompileError
 * @brief Error payload for compilation errors.
 */
struct CompileError {};

/**
 * @struct SaveError
 * @brief Error payload for file save errors.
 */
struct SaveError {};

/**
 * @struct LoadError
 * @brief Error payload for file load errors.
 */
struct LoadError {};

/**
 * @struct RuntimeError
 * @brief Error payload for runtime errors.
 */
struct RuntimeError {};

/**
 * @brief Variant type for all possible error payloads.
 */
using ErrorPayload = std::variant<SaveError, LoadError, CompileError, RuntimeError>;

}  // namespace nsbaci::types

#endif  // NSBACI_TYPES_ERRORTYPES_H
