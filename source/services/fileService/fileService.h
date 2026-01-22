/**
 * @file fileService.h
 * @brief FileService class declaration for nsbaci.
 *
 * This module defines the FileService class and its associated result types
 * for handling file system operations. The FileService provides a clean
 * abstraction over file I/O operations, specifically tailored for Nsbaci source
 * files (.nsb extension).
 *
 * The service handles:
 * - Saving source code to .nsb files with validation
 * - Loading source code from .nsb files with error checking
 * - Path validation
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_FILESERVICE_H
#define NSBACI_FILESERVICE_H

#include <vector>

#include "baseResult.h"
#include "fileTypes.h"

/**
 * @struct FileResult
 * @brief Base result type for file operations.
 *
 * Extends BaseResult with file-specific semantics. All file operation results
 * inherit from this type.
 *
 * @invariant If ok is false, errors vector contains at least one error.
 */
struct FileResult : nsbaci::BaseResult {
  /**
   * @brief Default constructor creates a successful result.
   */
  FileResult() : BaseResult() {}

  /**
   * @brief Constructs a result from a vector of errors.
   * @param errs Vector of errors encountered during the operation.
   */
  explicit FileResult(std::vector<nsbaci::Error> errs)
      : BaseResult(std::move(errs)) {}

  /**
   * @brief Constructs a failed result from a single error.
   * @param error The error that caused the operation to fail.
   */
  explicit FileResult(nsbaci::Error error) : BaseResult(std::move(error)) {}

  FileResult(FileResult&&) noexcept = default;
  FileResult& operator=(FileResult&&) noexcept = default;

  FileResult(const FileResult&) = default;
  FileResult& operator=(const FileResult&) = default;
};

/**
 * @struct saveResult
 * @brief Result type for file save operations.
 *
 * Contains only success/failure status and error information since save
 * operations do not return additional data on success.
 */
struct saveResult : FileResult {
  /**
   * @brief Default constructor creates a successful result.
   */
  saveResult() : FileResult() {}

  /**
   * @brief Constructs a result from a vector of errors.
   * @param errs Vector of errors encountered during the save.
   */
  explicit saveResult(std::vector<nsbaci::Error> errs)
      : FileResult(std::move(errs)) {}

  /**
   * @brief Constructs a failed result from a single error.
   * @param error The error that caused the save to fail.
   */
  explicit saveResult(nsbaci::Error error) : FileResult(std::move(error)) {}

  saveResult(saveResult&&) noexcept = default;
  saveResult& operator=(saveResult&&) noexcept = default;
  saveResult(const saveResult&) = default;
  saveResult& operator=(const saveResult&) = default;
};

/**
 * @struct LoadResult
 * @brief Result type for file load operations.
 *
 * Extends FileResult with the loaded file contents and filename.
 * On successful load, contains the file contents as a string and the
 * filename for display purposes.
 */
struct LoadResult : FileResult {
  /**
   * @brief Default constructor creates a successful but empty result.
   */
  LoadResult() : FileResult() {}

  /**
   * @brief Constructs a successful result with file contents.
   * @param conts The loaded file contents.
   * @param name The filename (without path) for display.
   */
  LoadResult(nsbaci::types::Text conts, nsbaci::types::File name)
      : FileResult(), contents(std::move(conts)), fileName(std::move(name)) {}

  /**
   * @brief Constructs a result from a vector of errors.
   * @param errs Vector of errors encountered during the load.
   */
  explicit LoadResult(std::vector<nsbaci::Error> errs)
      : FileResult(std::move(errs)) {}

  /**
   * @brief Constructs a failed result from a single error.
   * @param error The error that caused the load to fail.
   */
  explicit LoadResult(nsbaci::Error error) : FileResult(std::move(error)) {}

  LoadResult(LoadResult&&) noexcept = default;
  LoadResult& operator=(LoadResult&&) noexcept = default;
  LoadResult(const LoadResult&) = default;
  LoadResult& operator=(const LoadResult&) = default;

  nsbaci::types::Text contents;  ///< The loaded file contents.
  nsbaci::types::File fileName;  ///< The filename for display purposes.
};

/**
 * @namespace nsbaci::services
 * @brief Services namespace containing all backend service implementations.
 *
 * This namespace contains service classes that encapsulate specific
 * functionality areas such as file I/O, compilation, and runtime execution.
 */
namespace nsbaci::services {

/**
 * @class FileService
 * @brief Service for handling file system operations on BACI source files.
 *
 * FileService provides methods for saving and loading BACI source code files.
 * It enforces the .nsb file extension and provides detailed error reporting
 * for various failure scenarios including:
 *
 * - Empty or invalid file paths
 * - Invalid file extensions (must be .nsb)
 * - Non-existent directories or files
 * - Permission and I/O errors
 *
 * The service is designed to be move-only (non-copyable) to ensure single
 * ownership and prevent accidental resource duplication.
 *
 * Usage example:
 * @code
 * FileService fs;
 *
 * // Save a file
 * auto saveRes = fs.save(sourceCode, "program.nsb");
 * if (!saveRes.ok) {
 *   // Handle save error
 * }
 *
 * // Load a file
 * auto loadRes = fs.load("program.nsb");
 * if (loadRes.ok) {
 *   std::string code = loadRes.contents;
 * }
 * @endcode
 */
class FileService {
 public:
  /**
   * @brief Saves source code content to a file.
   *
   * Validates the file path and extension before attempting to write.
   * Creates or overwrites the target file with the provided contents.
   *
   * @param contents The source code text to save.
   * @param file The target file path (must have .nsb extension).
   * @return saveResult indicating success or containing error details.
   *
   * @note The parent directory must exist; this method does not create
   * directories.
   */
  saveResult save(nsbaci::types::Text contents, nsbaci::types::File file);

  /**
   * @brief Loads source code content from a file.
   *
   * Validates the file path, extension, and existence before reading.
   * Returns the complete file contents on success.
   *
   * @param file The source file path to load (must have .nsb extension).
   * @return LoadResult containing file contents on success, or error details on
   * failure.
   */
  LoadResult load(nsbaci::types::File file);

  /**
   * @brief Default constructor.
   */
  FileService() = default;

  FileService(const FileService&) = delete;
  FileService& operator=(const FileService&) = delete;

  FileService(FileService&&) = default;
  FileService& operator=(FileService&&) = default;

  ~FileService() = default;
};

}  // namespace nsbaci::services

#endif  // NSBACI_FILESERVICE_H
