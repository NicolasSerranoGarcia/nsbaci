/**
 * @file runtimeService.cpp
 * @brief Implementation unit for the RuntimeService.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "runtimeService.h"

namespace nsbaci::services {
RuntimeService::RuntimeService(std::unique_ptr<runtime::Interpreter> i,
                               std::unique_ptr<runtime::Scheduler> s,
                               std::unique_ptr<runtime::Program> p)
    : interpreter(std::move(i)),
      scheduler(std::move(s)),
      program(std::move(p)) {}

void RuntimeService::changeProgram(std::unique_ptr<runtime::Program> p) {
  program.release();
  program = std::move(p);
}
}  // namespace nsbaci::services
