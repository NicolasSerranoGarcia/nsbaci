/**
 * @file uiError.cpp
 * @brief Implementation of UIError utilities.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "uiError.h"

namespace nsbaci {

std::vector<UIError> UIError::fromBackendErrors(
    const std::vector<Error>& errors) {
  std::vector<UIError> uiErrors;
  uiErrors.reserve(errors.size());

  for (const auto& err : errors) {
    UIError uiErr;

    uiErr.severity = err.basic.severity;
    uiErr.title = titleFromPayload(err.payload);

    // Build body: main message + reason + context
    QString reason = reasonFromType(err.basic.type);
    QString context = contextFromPayload(err.payload);

    uiErr.body = QString::fromStdString(err.basic.message);
    if (!reason.isEmpty()) {
      uiErr.body += QString("\n\nReason: %1").arg(reason);
    }
    if (!context.isEmpty()) {
      uiErr.body += QString("\n\n%1").arg(context);
    }

    uiErrors.push_back(std::move(uiErr));
  }

  return uiErrors;
}

QString UIError::reasonFromType(ErrType type) {
  switch (type) {
    case ErrType::emptyPath:
      return "No file path was provided.";
    case ErrType::invalidPath:
      return "The file path is malformed or invalid.";
    case ErrType::invalidExtension:
      return "Only .nsb files are supported.";
    case ErrType::directoryNotFound:
      return "The target directory does not exist.";
    case ErrType::fileNotFound:
      return "The file does not exist.";
    case ErrType::notARegularFile:
      return "The path does not point to a regular file.";
    case ErrType::permissionDenied:
      return "Permission denied. Try running the application with elevated "
             "privileges.";
    case ErrType::openFailed:
      return "Could not open the file. It may be in use by another program.";
    case ErrType::readFailed:
      return "An error occurred while reading the file.";
    case ErrType::writeFailed:
      return "An error occurred while writing to the file.";
    case ErrType::unknown:
    default:
      return "An unknown error occurred.";
  }
}

QString UIError::titleFromPayload(const ErrorPayload& payload) {
  return std::visit(
      [](auto&& arg) -> QString {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, SaveError>) {
          return "Save Error";
        } else if constexpr (std::is_same_v<T, LoadError>) {
          return "Open Error";
        } else if constexpr (std::is_same_v<T, CompileError>) {
          return "Compilation Error";
        } else if constexpr (std::is_same_v<T, RuntimeError>) {
          return "Runtime Error";
        } else {
          return "Error";
        }
      },
      payload);
}

QString UIError::contextFromPayload(const ErrorPayload& payload) {
  return std::visit(
      [](auto&& arg) -> QString {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, SaveError>) {
          if (!arg.associatedFile.empty()) {
            return QString("File: %1")
                .arg(QString::fromStdString(arg.associatedFile.string()));
          }
        } else if constexpr (std::is_same_v<T, LoadError>) {
          if (!arg.associatedFile.empty()) {
            return QString("File: %1")
                .arg(QString::fromStdString(arg.associatedFile.string()));
          }
        }
        return QString();
      },
      payload);
}

}  // namespace nsbaci
