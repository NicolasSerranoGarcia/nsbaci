/**
 * @file compilerServiceFactory.h
 * @brief CompilerServiceFactory class declaration for nsbaci.
 *
 * This factory is responsible for creating CompilerService instances
 * based on configuration or runtime parameters.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_COMPILERSERVICEFACTORY_H
#define NSBACI_COMPILERSERVICEFACTORY_H

#include "compilerService.h"

/**
 * @namespace nsbaci::factories
 * @brief Factories namespace for nsbaci.
 */
namespace nsbaci::factories {

struct NsbaciCompiler {
  explicit NsbaciCompiler() = default;
};

// Tag used to generate a service with a nsbaci compiler
constexpr inline NsbaciCompiler nsbaciCompiler{};

/**
 * @class CompilerServiceFactory
 * @brief Factory for creating CompilerService instances.
 */
class CompilerServiceFactory {
 private:
  CompilerServiceFactory() = default;

 public:
  static nsbaci::services::CompilerService createService(NsbaciCompiler t);
  // static nsbaci::services::CompilerService createService(OtherCompilerOrTest
  // t);
  ~CompilerServiceFactory() = default;
};

}  // namespace nsbaci::factories

#endif  // NSBACI_COMPILERSERVICEFACTORY_H
