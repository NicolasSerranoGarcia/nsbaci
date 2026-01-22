/**
 * @file compilerServiceFactory.cpp
 * @brief Implementation unit for the CompilerServiceFactory.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "compilerServiceFactory.h"

namespace nsbaci::factories {

nsbaci::services::CompilerService CompilerServiceFactory::createService(
    NsbaciCompiler t) {
  // Default constructor generates a nsbaci compiler
  return nsbaci::services::CompilerService{};
}

}  // namespace nsbaci::factories
