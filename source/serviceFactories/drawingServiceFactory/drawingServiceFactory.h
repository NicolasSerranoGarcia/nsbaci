/**
 * @file drawingServiceFactory.h
 * @brief DrawingServiceFactory class declaration for nsbaci.
 *
 * This factory is responsible for creating DrawingService instances
 * based on configuration or runtime parameters.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_DRAWINGSERVICEFACTORY_H
#define NSBACI_DRAWINGSERVICEFACTORY_H

#include "drawingService.h"

/**
 * @namespace nsbaci::factories
 * @brief Factories namespace for nsbaci.
 */
namespace nsbaci::factories {

struct DefaultDrawingBackend {
  explicit DefaultDrawingBackend() = default;
};

// tag used to generate a service with the default drawing backend
constexpr inline DefaultDrawingBackend defaultDrawingBackend{};

/**
 * @class DrawingServiceFactory
 * @brief Factory for creating DrawingService instances.
 */
class DrawingServiceFactory {
 private:
  DrawingServiceFactory() = default;

 public:
  static nsbaci::services::DrawingService createService(
      DefaultDrawingBackend t);
  // static nsbaci::services::DrawingService createService(OtherBackendOrTest
  // t);
  ~DrawingServiceFactory() = default;
};

}  // namespace nsbaci::factories

#endif  // NSBACI_DRAWINGSERVICEFACTORY_H
