/**
 * @file fileTypes.h
 * @brief Type definitions for file-related operations.
 *
 * This header provides type aliases used by the FileService
 * and other components that work with files.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_TYPES_FILETYPES_H
#define NSBACI_TYPES_FILETYPES_H

#include <filesystem>
#include <string>

/**
 * @namespace nsbaci::types
 * @brief Type definitions namespace for nsbaci.
 */
namespace nsbaci::types {

namespace fs = std::filesystem;

/**
 * @brief Alias for text content (file contents, source code, etc.)
 */
using Text = std::string;

/**
 * @brief Alias for file system paths.
 */
using File = fs::path;

}  // namespace nsbaci::types

#endif  // NSBACI_TYPES_FILETYPES_H
