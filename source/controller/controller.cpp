/**
 * @file controller.cpp
 * @brief Implementation unit for the Controller.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "controller.h"

namespace nsbaci {

Controller::Controller(QObject* parent) : QObject(parent) {}

void Controller::onSaveRequested(File file, Text contents) {
  auto saveRes = fileService.save(contents, file);

  if (!saveRes.ok) {
    auto uiErrors = UIError::fromBackendErrors(saveRes.errors);
    emit saveFailed(std::move(uiErrors));
  } else {
    emit saveSucceeded();
  }
}

void Controller::onOpenRequested(File file) {
  auto openRes = fileService.load(file);

  if (!openRes.ok) {
    auto uiErrors = UIError::fromBackendErrors(openRes.errors);
    emit loadFailed(std::move(uiErrors));
  } else {
    emit loadSucceeded(QString::fromStdString(openRes.contents));
  }
}

void Controller::onCompileRequested(Text contents) {
  // TODO: Delegate to compilerService
}

void Controller::onRunRequested() {
  // TODO: Delegate to runtimeService
}

}  // namespace nsbaci
