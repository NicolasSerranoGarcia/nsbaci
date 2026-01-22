/**
 * @file runtimeServiceFactory.cpp
 * @brief Implementation unit for the RuntimeServiceFactory.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "runtimeServiceFactory.h"

#include "nsbaciInterpreter.h"
#include "nsbaciScheduler.h"

namespace nsbaci::factories {

nsbaci::services::RuntimeService RuntimeServiceFactory::createService(
    NsbaciRuntime) {
  auto interpreter =
      std::make_unique<nsbaci::services::runtime::NsbaciInterpreter>();
  auto scheduler =
      std::make_unique<nsbaci::services::runtime::NsbaciScheduler>();

  return nsbaci::services::RuntimeService{std::move(interpreter),
                                          std::move(scheduler)};
}

}  // namespace nsbaci::factories
