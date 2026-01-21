/**
 * @file drawingService.h
 * @brief DrawingService class declaration for nsbaci.
 *
 * This service acts as an adapter for graphical output backends.
 * It communicates with the RuntimeService via Qt signals/slots to handle
 * drawing operations triggered by program execution.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_SERVICES_DRAWINGSERVICE_H
#define NSBACI_SERVICES_DRAWINGSERVICE_H

#include <memory>

/**
 * @namespace nsbaci::services
 * @brief Services namespace for nsbaci.
 */
namespace nsbaci::services {

/**
 * @class DrawingService
 * @brief Adapter service for graphical output backends.
 */
class DrawingService {
 public:
  DrawingService() = default;
  ~DrawingService() = default;
  // TODO: Add signals for notifying backends of drawing operations
  // Example: void drawFigure(std::shared_ptr<Figure> figure);

 private:
  // TODO: Add backend reference/pointer when backends are implemented
};

}  // namespace nsbaci::services

#endif  // NSBACI_SERVICES_DRAWINGSERVICE_H