/**
 * @file uiError.h
 * @brief UI Error type definitions for nsbaci.
 *
 * This header provides types used to communicate between the controller
 * and the view layer, particularly for error display.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_UIERROR_H
#define NSBACI_UIERROR_H

#include <QString>
#include <vector>

#include "error.h"
#include "errorTypes.h"

namespace nsbaci {

/**
 * @struct UIError
 * @brief UI-ready error representation for display in dialogs.
 *
 * Contains all the information needed to render an error dialog:
 * title, body text, and severity (which maps to an icon).
 */
struct UIError {
  QString title;  // Dialog title (e.g., "Save Error", "Load Error")
  QString body;   // Main message body with details
  nsbaci::types::ErrSeverity severity;  // Maps to icon (Warning, Error, Fatal)

  /**
   * @brief Converts backend errors to UI-ready errors.
   * @param errors Vector of backend Error objects.
   * @return Vector of UIError objects ready for display.
   */
  static std::vector<UIError> fromBackendErrors(
      const std::vector<Error>& errors);

 private:
  /**
   * @brief Maps an ErrType to a human-readable reason string.
   */
  static QString reasonFromType(nsbaci::types::ErrType type);

  /**
   * @brief Gets a title based on the error payload type.
   */
  static QString titleFromPayload(const nsbaci::types::ErrorPayload& payload);

  /**
   * @brief Gets additional context from the error payload.
   */
  static QString contextFromPayload(const nsbaci::types::ErrorPayload& payload);
};

}  // namespace nsbaci

#endif  // NSBACI_UIERROR_H
