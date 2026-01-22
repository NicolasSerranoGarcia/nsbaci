/**
 * @file fileServiceFactory.h
 * @brief FileServiceFactory class declaration for nsbaci.
 *
 * This factory is responsible for creating FileService instances
 * based on configuration or runtime parameters.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_FILESERVICEFACTORY_H
#define NSBACI_FILESERVICEFACTORY_H

#include "fileService.h"

/**
 * @namespace nsbaci::factories
 * @brief Factories namespace for nsbaci.
 */
namespace nsbaci::factories {

struct DefaultFileSystem {
  explicit DefaultFileSystem() = default;
};

// Tag used to generate a service with the default file system
constexpr inline DefaultFileSystem defaultFileSystem{};

/**
 * @class FileServiceFactory
 * @brief Factory for creating FileService instances.
 */
class FileServiceFactory {
 private:
  FileServiceFactory() = default;

 public:
  static nsbaci::services::FileService createService(DefaultFileSystem t);
  // static nsbaci::services::FileService createService(OtherFileSystemOrTest
  // t);
  ~FileServiceFactory() = default;
};

}  // namespace nsbaci::factories

#endif  // NSBACI_FILESERVICEFACTORY_H
