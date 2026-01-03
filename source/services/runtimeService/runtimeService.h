/**
 * @file runtimeService.h
 * @brief RuntimeService class declaration for nsbaci.
 *
 * This service is intended to encapsulate runtime-related utilities.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_RUNTIMESERVICE_H
#define NSBACI_RUNTIMESERVICE_H

#include <memory>

#include "interpreter.h"
#include "program.h"
#include "scheduler.h"

/**
 * @namespace nsbaci::services
 * @brief Services namespace for nsbaci.
 */
namespace nsbaci::services {

/**
 * @class RuntimeService
 */
class RuntimeService {
 public:
  // in order to communicate things to the gui (for example, we can have a list
  // of variables currently created), the backend con send signals to the
  // service, telling him that a variable values has changed, for example. The
  // runtime service resend the info to the controller, maybe tweaking it in
  // some way. Finally, the controller knows exactly what and how to update in
  // the gui (maybe a device or object "Variable Table" in the controller that
  // knows what to tell the gui)
  RuntimeService() = default;
  RuntimeService(std::unique_ptr<runtime::Interpreter> i,
                 std::unique_ptr<runtime::Scheduler>
                     s);  // move them into the class attributes
  ~RuntimeService() = default;

 private:
  runtime::Program program;
  std::unique_ptr<runtime::Interpreter> interpreter;
  std::unique_ptr<runtime::Scheduler> scheduler;
};

}  // namespace nsbaci::services

#endif  // NSBACI_RUNTIMESERVICE_H
