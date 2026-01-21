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

#include "baseResult.h"
#include "fileTypes.h"

// guarantee: if ok == false, errors.size() > 0
struct FileResult : nsbaci::BaseResult {
  FileResult() : BaseResult() {}
  explicit FileResult(std::vector<nsbaci::Error> errs)
      : BaseResult(std::move(errs)) {}
  explicit FileResult(nsbaci::Error error)
      : BaseResult(std::move(error)) {}

  FileResult(FileResult&&) noexcept = default;
  FileResult& operator=(FileResult&&) noexcept = default;

  FileResult(const FileResult&) = default;
  FileResult& operator=(const FileResult&) = default;
};

struct saveResult : FileResult {
  saveResult() : FileResult() {}
  explicit saveResult(std::vector<nsbaci::Error> errs)
      : FileResult(std::move(errs)) {}
  explicit saveResult(nsbaci::Error error) : FileResult(std::move(error)) {}

  saveResult(saveResult&&) noexcept = default;
  saveResult& operator=(saveResult&&) noexcept = default;
  saveResult(const saveResult&) = default;
  saveResult& operator=(const saveResult&) = default;
};

struct LoadResult : FileResult {
  LoadResult() : FileResult() {}
  LoadResult(nsbaci::types::Text conts, nsbaci::types::File name)
      : FileResult(), contents(std::move(conts)), fileName(std::move(name)) {}
  explicit LoadResult(std::vector<nsbaci::Error> errs)
      : FileResult(std::move(errs)) {}
  explicit LoadResult(nsbaci::Error error) : FileResult(std::move(error)) {}

  LoadResult(LoadResult&&) noexcept = default;
  LoadResult& operator=(LoadResult&&) noexcept = default;
  LoadResult(const LoadResult&) = default;
  LoadResult& operator=(const LoadResult&) = default;

  nsbaci::types::Text contents;
  nsbaci::types::File fileName;
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
  saveResult save(nsbaci::types::Text contents, nsbaci::types::File file);

  LoadResult load(nsbaci::types::File file);

  FileService() = default;

  FileService(const FileService&) = delete; 
  FileService& operator=(const FileService&) = delete;

  FileService(FileService&&) = default;
  FileService& operator=(FileService&&) = default;

  ~FileService() = default;
};

}  // namespace nsbaci::services

#endif  // NSBACI_FILESERVICE_H
