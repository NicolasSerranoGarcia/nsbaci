/**
 * @file drawingServiceFactory.cpp
 * @brief Implementation unit for the DrawingServiceFactory.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "drawingServiceFactory.h"

#include <memory>

namespace nsbaci::factories {

std::unique_ptr<nsbaci::services::DrawingService> DrawingServiceFactory::createService(
    DefaultDrawingBackend) {
  // default constructor generates a default drawing service
  return std::make_unique<nsbaci::services::DrawingService>();
}

}  // namespace nsbaci::factories
