/**
 * @file fileService.cpp
 * @brief Implementation of the FileService class for nsbaci.
 *
 * This file contains the implementation of file save and load operations
 * for NsBaci source files (.nsb). It provides comprehensive validation and
 * error handling for all file system operations.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "fileService.h"

#include <fstream>

using namespace nsbaci::types;

namespace nsbaci::services {

saveResult FileService::save(Text contents, File file) {
  // Validate file path
  if (file.empty()) {
    Error err;
    err.basic.severity = ErrSeverity::Error;
    err.basic.message = "File path is empty.";
    err.basic.type = ErrType::emptyPath;
    err.payload = SaveError{file};
    return saveResult(err);
  }

  // Validate .nsb extension
  if (file.extension() != ".nsb") {
    Error err;
    err.basic.severity = ErrSeverity::Error;
    err.basic.message =
        "Invalid file extension. Only .nsb files are supported.";
    err.basic.type = ErrType::invalidExtension;
    err.payload = SaveError{file};
    return saveResult(err);
  }

  // Get parent directory
  File parentDir = file.parent_path();

  // Check if parent directory exists (if specified)
  if (!parentDir.empty() && !fs::exists(parentDir)) {
    Error err;
    err.basic.severity = ErrSeverity::Error;
    err.basic.message = "Directory does not exist: " + parentDir.string();
    err.basic.type = ErrType::directoryNotFound;
    err.payload = SaveError{file};
    return saveResult(err);
  }

  // Try to open file for writing
  std::ofstream outFile(file, std::ios::out | std::ios::trunc);

  if (!outFile.is_open()) {
    Error err;
    err.basic.severity = ErrSeverity::Error;
    err.basic.message = "Could not open file for writing: " + file.string();
    err.basic.type = ErrType::openFailed;
    err.payload = SaveError{file};
    return saveResult(err);
  }

  // Write contents
  outFile << contents;

  // Check for write errors
  if (outFile.fail()) {
    Error err;
    err.basic.severity = ErrSeverity::Error;
    err.basic.message = "Failed to write to file: " + file.string();
    err.basic.type = ErrType::writeFailed;
    err.payload = SaveError{file};
    return saveResult(err);
  }

  outFile.close();

  // Success - return empty result (ok = true)
  return saveResult();
}

LoadResult FileService::load(File file) {
  // Validate file path
  if (file.empty()) {
    Error err;
    err.basic.severity = ErrSeverity::Error;
    err.basic.message = "File path is empty.";
    err.basic.type = ErrType::emptyPath;
    err.payload = LoadError{file};
    return LoadResult(err);
  }

  // Validate .nsb extension
  if (file.extension() != ".nsb") {
    Error err;
    err.basic.severity = ErrSeverity::Error;
    err.basic.message =
        "Invalid file extension. Only .nsb files are supported.";
    err.basic.type = ErrType::invalidExtension;
    err.payload = LoadError{file};
    return LoadResult(err);
  }

  // Check if file exists
  if (!fs::exists(file)) {
    Error err;
    err.basic.severity = ErrSeverity::Error;
    err.basic.message = "File does not exist: " + file.string();
    err.basic.type = ErrType::fileNotFound;
    err.payload = LoadError{file};
    return LoadResult(err);
  }

  // Check if it's a regular file
  if (!fs::is_regular_file(file)) {
    Error err;
    err.basic.severity = ErrSeverity::Error;
    err.basic.message = "Path is not a regular file: " + file.string();
    err.basic.type = ErrType::notARegularFile;
    err.payload = LoadError{file};
    return LoadResult(err);
  }

  // Try to open file for reading
  std::ifstream inFile(file);

  if (!inFile.is_open()) {
    Error err;
    err.basic.severity = ErrSeverity::Error;
    err.basic.message = "Could not open file for reading: " + file.string();
    err.basic.type = ErrType::openFailed;
    err.payload = LoadError{file};
    return LoadResult(err);
  }

  // Read entire file contents
  Text contents((std::istreambuf_iterator<char>(inFile)),
                std::istreambuf_iterator<char>());

  if (inFile.fail() && !inFile.eof()) {
    Error err;
    err.basic.severity = ErrSeverity::Error;
    err.basic.message = "Failed to read file: " + file.string();
    err.basic.type = ErrType::readFailed;
    err.payload = LoadError{file};
    return LoadResult(err);
  }

  inFile.close();

  // Success - return contents and filename
  return LoadResult(std::move(contents), file.filename());
}

}  // namespace nsbaci::services
