/**
 * @file runtimeServiceFactory.h
 * @brief RuntimeServiceFactory class declaration for nsbaci.
 *
 * This factory is responsible for creating RuntimeService instances
 * based on configuration or runtime parameters.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_RUNTIMESERVICEFACTORY_H
#define NSBACI_RUNTIMESERVICEFACTORY_H

#include "runtimeService.h"

/**
 * @namespace nsbaci::factories
 * @brief Factories namespace for nsbaci.
 */
namespace nsbaci::factories {

struct NsbaciRuntime {
  explicit NsbaciRuntime() = default;
};

// Tag used to generate a service with a nsbaci runtime
constexpr inline NsbaciRuntime nsbaciRuntime{};

/**
 * @class RuntimeServiceFactory
 * @brief Factory for creating RuntimeService instances.
 */
class RuntimeServiceFactory {
 private:
  RuntimeServiceFactory() = default;

 public:
  static nsbaci::services::RuntimeService createService(NsbaciRuntime t);
  // static nsbaci::services::RuntimeService createService(OtherRuntimeOrTest
  // t);
  ~RuntimeServiceFactory() = default;
};

}  // namespace nsbaci::factories

#endif  // NSBACI_RUNTIMESERVICEFACTORY_H
