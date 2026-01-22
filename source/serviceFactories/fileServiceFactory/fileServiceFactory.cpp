/**
 * @file fileServiceFactory.cpp
 * @brief Implementation unit for the FileServiceFactory.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "fileServiceFactory.h"

namespace nsbaci::factories {

nsbaci::services::FileService FileServiceFactory::createService(
    DefaultFileSystem t) {
  // Default constructor generates a default file service
  return nsbaci::services::FileService{};
}

}  // namespace nsbaci::factories
