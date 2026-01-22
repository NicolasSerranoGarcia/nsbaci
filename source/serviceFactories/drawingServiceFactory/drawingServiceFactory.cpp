/**
 * @file drawingServiceFactory.cpp
 * @brief Implementation unit for the DrawingServiceFactory.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "drawingServiceFactory.h"

namespace nsbaci::factories {

nsbaci::services::DrawingService DrawingServiceFactory::createService(
    DefaultDrawingBackend t) {
  // default constructor generates a default drawing service
  return nsbaci::services::DrawingService{};
}

}  // namespace nsbaci::factories
