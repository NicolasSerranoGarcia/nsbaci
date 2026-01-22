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

#include <string>
#include <variant>

#include "fileTypes.h"

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
 * @enum ErrType
 * @brief Types of errors that can occur in the application.
 */
enum class ErrType {
  // File path errors
  emptyPath,          // Path string is empty
  invalidPath,        // Path is malformed or invalid
  invalidExtension,   // File does not have .nsb extension
  directoryNotFound,  // Parent directory doesn't exist
  fileNotFound,       // File doesn't exist
  notARegularFile,    // Path points to directory, symlink, etc.

  // Permission errors
  permissionDenied,  // No read/write access

  // I/O errors
  openFailed,   // Could not open file
  readFailed,   // Error while reading
  writeFailed,  // Error while writing

  // Compilation errors
  compilationError,  // Syntax or semantic error during compilation

  // Generic
  unknown  // Unspecified error
};

using ErrMessage = std::string;

/**
 * @struct ErrorBase
 * @brief Base structure containing common error properties.
 */
struct ErrorBase {
  // this severity serves as an indicator to what icon to use in the error, the
  // string that appears in the top of the dialog, additional buttons for things
  // like restart when the error is fatal...
  ErrSeverity severity;
  ErrMessage message;
  // useful to show the reason for things. For example, if when trying to save a
  // file to a sensible location, if the app doesn't have privileges, the ui can
  // show a message of type: "/Error/ X could not be saved in Y. Reason:
  // Permission Denied. Try starting the app in admin mode. [Ok]"
  ErrType type;
};

/**
 * @struct CompileError
 * @brief Error payload for compilation errors.
 */
struct CompileError {
  int line = 0;
  int column = 0;
};

/**
 * @struct SaveError
 * @brief Error payload for file save errors.
 */
struct SaveError {
  File associatedFile;
};

/**
 * @struct LoadError
 * @brief Error payload for file load errors.
 */
struct LoadError {
  File associatedFile;
};

/**
 * @struct RuntimeError
 * @brief Error payload for runtime errors.
 */
struct RuntimeError {};

/**
 * @brief Variant type for all possible error payloads.
 * Can be used to create a more specific error message in the controller, for
 * example when an error is of type compileError, the controller can specify the
 * line, col, what might have happened...
 */
using ErrorPayload =
    std::variant<SaveError, LoadError, CompileError, RuntimeError>;

}  // namespace nsbaci::types

#endif  // NSBACI_TYPES_ERRORTYPES_H
