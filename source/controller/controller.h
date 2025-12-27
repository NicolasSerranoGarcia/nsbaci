/**
 * @file controller.h
 * @brief Controller class declaration for nsbaci.
 *
 * This module defines the control layer placeholder.
 * The Controller class is intended to coordinate the UI and services.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_CONTROLLER_H
#define NSBACI_CONTROLLER_H

#include "fileService.h"
#include "fileTypes.h"

using namespace nsbaci::services;
using namespace nsbaci::types;

/**
 * @namespace nsbaci
 * @brief Root namespace for the nsbaci application.
 */
namespace nsbaci {

/**
 * @class Controller
 */
class Controller {
 public:
  // gets called with the full path, including the name
  void save(File file, Text contents);

  // Usually gets called inside save, if the file is not already saved
  void saveAs(File file, Text contents);

  Controller() = default;
  ~Controller() = default;

 private:
  FileService fileService;
};

}  // namespace nsbaci

#endif  // NSBACI_CONTROLLER_H
