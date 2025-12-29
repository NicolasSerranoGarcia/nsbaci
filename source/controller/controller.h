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

#include <QObject>

#include "compilerService.h"
#include "compilerTypes.h"
#include "fileService.h"
#include "fileTypes.h"
#include "runtimeService.h"
#include "runtimeTypes.h"
#include "uiError.h"

/**
 * @namespace nsbaci
 * @brief Root namespace for the nsbaci application.
 */
namespace nsbaci {

/**
 * @class Controller
 */
class Controller : public QObject {
  Q_OBJECT

 public:
  explicit Controller(QObject* parent = nullptr);
  ~Controller() = default;

 signals:
  // File operation results
  void saveFailed(std::vector<UIError> errors);
  void saveSucceeded();
  void loadFailed(std::vector<UIError> errors);
  void loadSucceeded(const QString& contents);

 public slots:
  // File operations
  void onSaveRequested(nsbaci::types::File file, nsbaci::types::Text contents);
  void onOpenRequested(nsbaci::types::File file);

  // Build operations
  void onCompileRequested(nsbaci::types::Text contents);
  void onRunRequested();

 private:
  nsbaci::services::FileService fileService;
  nsbaci::services::CompilerService compilerService;
  nsbaci::services::RuntimeService runtimeService;
};

}  // namespace nsbaci

#endif  // NSBACI_CONTROLLER_H
