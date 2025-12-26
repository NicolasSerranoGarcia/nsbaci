/**
 * @file fileService.h
 * @brief FileService class declaration for nsbaci.
 *
 * This service is intended to encapsulate file system operations.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_FILESERVICE_H
#define NSBACI_FILESERVICE_H

#include <vector>

#include "error.h"
#include "fileTypes.h"

using namespace nsbaci;
using namespace nsbaci::types;

//guarantee: if ok == false, errors.size() > 0
struct FileResult {
  FileResult() : ok(true) {}
  explicit FileResult(std::vector<Error> errs)
      : ok(errs.empty()), errors(std::move(errs)) {}
  explicit FileResult(Error error) : ok(false), errors({std::move(error)}) {}

  FileResult(FileResult&&) noexcept = default;
  FileResult& operator=(FileResult&&) noexcept = default;

  FileResult(const FileResult&) = default;
  FileResult& operator=(const FileResult&) = default;

  bool ok;
  std::vector<Error> errors;
};

struct saveResult : FileResult {
  saveResult() : FileResult() {}
  explicit saveResult(std::vector<Error> errs) : FileResult(std::move(errs)) {}
  explicit saveResult(Error error) : FileResult(std::move(error)) {}

  saveResult(saveResult&&) noexcept = default;
  saveResult& operator=(saveResult&&) noexcept = default;
  saveResult(const saveResult&) = default;
  saveResult& operator=(const saveResult&) = default;
};

struct LoadResult : FileResult {
  LoadResult() : FileResult() {}
  LoadResult(Text conts, File name)
      : FileResult(), contents(std::move(conts)), fileName(std::move(name)) {}
  explicit LoadResult(std::vector<Error> errs) : FileResult(std::move(errs)) {}
  explicit LoadResult(Error error) : FileResult(std::move(error)) {}

  LoadResult(LoadResult&&) noexcept = default;
  LoadResult& operator=(LoadResult&&) noexcept = default;
  LoadResult(const LoadResult&) = default;
  LoadResult& operator=(const LoadResult&) = default;

  Text contents;
  File fileName;
};

/**
 * @namespace nsbaci::services
 * @brief Services namespace for nsbaci.
 */
namespace nsbaci::services {

/**
 * @class FileService
 */
class FileService {
 public:

  saveResult save(Text contents, File file);

  LoadResult load(File file);

  FileService() = default;

  ~FileService() = default;
};

}  // namespace nsbaci::services

#endif  // NSBACI_FILESERVICE_H
