/**
 * @file errorDialogFactory.h
 * @brief Factory for creating error dialogs from UIError objects.
 *
 * This module provides a factory that converts UIError objects into
 * ready-to-show QMessageBox dialogs. It is private to the UI layer.
 *
 * ## Overview
 *
 * The ErrorDialogFactory operates in two modes:
 *
 * ### 1. Deferred Mode (Factory Pattern)
 *
 * Returns a **DialogInvoker** - a callable (`std::function`) that encapsulates
 * all dialog data but does NOT show the dialog immediately. The caller decides
 * when to invoke it. This is similar to a "lazy evaluation" or "thunk" pattern.
 *
 * @code
 * // Create the invoker (dialog is NOT shown yet)
 * auto dialogInvoker = ErrorDialogFactory::getDialogFromUIError(error, this);
 *
 * // ... do other work, validation, logging, etc.
 *
 * // Show the dialog when ready (blocks until user clicks)
 * QMessageBox::StandardButton clicked = dialogInvoker();
 *
 * // React to user choice
 * if (clicked == QMessageBox::Close) {
 *     QApplication::quit();
 * }
 * @endcode
 *
 * ### 2. Immediate Mode (Convenience)
 *
 * Shows the dialog immediately and returns the result. Internally, this
 * creates an invoker and calls it right away. Use this when you don't need
 * to defer the dialog display.
 *
 * @code
 * // Show immediately - blocks until user clicks
 * ErrorDialogFactory::showError(error, this);
 *
 * // Or capture the result
 * auto clicked = ErrorDialogFactory::showError(error, this);
 * @endcode
 *
 * ## Why Return Callables?
 *
 * The DialogInvoker pattern provides:
 * - **Separation of concerns**: Factory knows HOW to build, caller knows WHEN to show
 * - **Deferred execution**: Prepare dialogs ahead of time, show when appropriate
 * - **User response handling**: The return value indicates which button was clicked
 * - **Flexibility**: Batch prepare multiple dialogs, show conditionally, etc.
 *
 * The callable captures all necessary data by value, so the original UIError
 * can be destroyed before the dialog is shown.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_ERRORDIALOGFACTORY_H
#define NSBACI_ERRORDIALOGFACTORY_H

#include <QMessageBox>
#include <QWidget>
#include <functional>
#include <memory>
#include <vector>

#include "uiError.h"

namespace nsbaci::ui {

/**
 * @class ErrorDialogFactory
 * @brief Factory for creating error dialogs from UIError objects.
 *
 * Provides static methods to convert UIError objects into QMessageBox
 * dialogs. Supports two modes:
 * - **Deferred**: Returns a DialogInvoker callable for later invocation
 * - **Immediate**: Shows the dialog right away via convenience methods
 *
 * @see DialogInvoker
 */
class ErrorDialogFactory {
 public:
  /**
   * @brief Callable type that shows a dialog when invoked.
   *
   * A DialogInvoker is a packaged dialog that can be invoked at any time.
   * When called, it displays the dialog (blocking) and returns which
   * button the user clicked. Think of it as a "suspended dialog" or
   * "dialog builder" that you can trigger when ready.
   *
   * @return The button that was clicked (QMessageBox::StandardButton).
   */
  using DialogInvoker = std::function<QMessageBox::StandardButton()>;


  /**
   * @brief Creates a dialog invoker from a UIError.
   *
   * Returns a callable that, when invoked, shows the error dialog
   * and returns the button clicked by the user.
   *
   * @param error The UIError to display.
   * @param parent Parent widget for the dialog.
   * @return A callable that shows the dialog when invoked.
   */
  static DialogInvoker getDialogFromUIError(const UIError& error,
                                            QWidget* parent = nullptr);

  /**
   * @brief Creates dialog invokers for multiple UIErrors.
   *
   * @param errors Vector of UIErrors to convert.
   * @param parent Parent widget for all dialogs.
   * @return Vector of callables, one per error.
   */
  static std::vector<DialogInvoker> getDialogsFromUIErrors(
      const std::vector<UIError>& errors, QWidget* parent = nullptr);

  /**
   * @brief Creates a success message dialog invoker.
   *
   * @param title Dialog title.
   * @param message Success message body.
   * @param parent Parent widget for the dialog.
   * @return A callable that shows the success dialog when invoked.
   */
  static DialogInvoker getSuccessDialog(const QString& title,
                                        const QString& message,
                                        QWidget* parent = nullptr);


                                        

  /**
   * @brief Shows all error dialogs sequentially.
   *
   * Convenience method that creates and immediately shows dialogs
   * for all provided UIErrors.
   *
   * @param errors Vector of UIErrors to display.
   * @param parent Parent widget for all dialogs.
   */
  static void showErrors(const std::vector<UIError>& errors,
                         QWidget* parent = nullptr);

  /**
   * @brief Shows a single error dialog immediately.
   *
   * @param error The UIError to display.
   * @param parent Parent widget for the dialog.
   * @return The button that was clicked.
   */
  static QMessageBox::StandardButton showError(const UIError& error,
                                               QWidget* parent = nullptr);

  /**
   * @brief Shows a success message dialog immediately.
   *
   * @param title Dialog title.
   * @param message Success message body.
   * @param parent Parent widget for the dialog.
   */
  static void showSuccess(const QString& title, const QString& message,
                          QWidget* parent = nullptr);

 private:
  /**
   * @brief Maps ErrSeverity to QMessageBox::Icon.
   */
  static QMessageBox::Icon iconFromSeverity(ErrSeverity severity);
};

}  // namespace nsbaci::ui

#endif  // NSBACI_ERRORDIALOGFACTORY_H
