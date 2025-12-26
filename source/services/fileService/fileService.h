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

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

using Text = std::string;
using File = fs::path;

//guarantee: if ok == false, errors.size() > 0
struct FileResult{
  bool ok;
  std::vector<Error> errors;
};

struct saveResult : FileResult{

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

  FileService() = default;

  ~FileService() = default;
};

}  // namespace nsbaci::services

#endif  // NSBACI_FILESERVICE_H
