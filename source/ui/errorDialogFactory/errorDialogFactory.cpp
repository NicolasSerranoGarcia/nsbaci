/**
 * @file errorDialogFactory.cpp
 * @brief Implementation of ErrorDialogFactory.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "errorDialogFactory.h"

namespace nsbaci::ui {

ErrorDialogFactory::DialogInvoker ErrorDialogFactory::getDialogFromUIError(
    const UIError& error, QWidget* parent) {
  // Capture by value to ensure data lives beyond this function
  QString title = error.title;
  QString body = error.body;
  ErrSeverity severity = error.severity;

  return [parent, title, body, severity]() -> QMessageBox::StandardButton {
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(body);
    msgBox.setIcon(iconFromSeverity(severity));
    msgBox.setStandardButtons(QMessageBox::Ok);

    if (severity == ErrSeverity::Fatal) {
      msgBox.addButton(QMessageBox::Close);
      msgBox.setDefaultButton(QMessageBox::Close);
    } else {
      msgBox.setDefaultButton(QMessageBox::Ok);
    }

    msgBox.exec();
    return static_cast<QMessageBox::StandardButton>(msgBox.result());
  };
}

std::vector<ErrorDialogFactory::DialogInvoker>
ErrorDialogFactory::getDialogsFromUIErrors(const std::vector<UIError>& errors,
                                           QWidget* parent) {
  std::vector<DialogInvoker> invokers;
  invokers.reserve(errors.size());

  for (const auto& error : errors) {
    invokers.push_back(getDialogFromUIError(error, parent));
  }

  return invokers;
}

void ErrorDialogFactory::showErrors(const std::vector<UIError>& errors,
                                    QWidget* parent) {
  for (const auto& error : errors) {
    showError(error, parent);
  }
}

QMessageBox::StandardButton ErrorDialogFactory::showError(const UIError& error,
                                                          QWidget* parent) {
  auto invoker = getDialogFromUIError(error, parent);
  return invoker();
}

ErrorDialogFactory::DialogInvoker ErrorDialogFactory::getSuccessDialog(
    const QString& title, const QString& message, QWidget* parent) {
  return [parent, title, message]() -> QMessageBox::StandardButton {
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    return static_cast<QMessageBox::StandardButton>(msgBox.result());
  };
}

void ErrorDialogFactory::showSuccess(const QString& title,
                                     const QString& message, QWidget* parent) {
  auto invoker = getSuccessDialog(title, message, parent);
  invoker();
}

QMessageBox::Icon ErrorDialogFactory::iconFromSeverity(ErrSeverity severity) {
  switch (severity) {
    case ErrSeverity::Warning:
      return QMessageBox::Warning;
    case ErrSeverity::Error:
      return QMessageBox::Critical;
    case ErrSeverity::Fatal:
      return QMessageBox::Critical;
    default:
      return QMessageBox::Information;
  }
}

}  // namespace nsbaci::ui
