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

#include <QObject>

#include <memory>

#include "drawingTypes.h"

/**
 * @namespace nsbaci::services
 * @brief Services namespace for nsbaci.
 */
namespace nsbaci::services {

/**
 * @class DrawingService
 * @brief Adapter service for graphical output backends.
 *
 * The DrawingService manages drawing state and emits signals when drawing
 * operations are requested by the runtime. It follows an SDL-like approach
 * where you set the render color and then draw shapes.
 *
 * Usage pattern:
 * 1. setColor(r, g, b) - Set the current drawing color
 * 2. drawCircle/drawRect/etc - Draw shapes with current color
 * 3. clear() - Clear the canvas
 *
 * The service emits signals that can be connected to any drawing backend
 * (Qt widget, OpenGL, etc.)
 */
class DrawingService : public QObject {
  Q_OBJECT

 public:
  explicit DrawingService(QObject* parent = nullptr);
  ~DrawingService() = default;

  // QObject subclasses cannot be copied or moved
  DrawingService(const DrawingService&) = delete;
  DrawingService& operator=(const DrawingService&) = delete;
  DrawingService(DrawingService&&) = delete;
  DrawingService& operator=(DrawingService&&) = delete;

  // ============== Color Management ==============

  /**
   * @brief Set the current drawing color using RGB values.
   * @param r Red component (0-255)
   * @param g Green component (0-255)
   * @param b Blue component (0-255)
   */
  void setColor(uint8_t r, uint8_t g, uint8_t b);

  /**
   * @brief Set the current drawing color using RGBA values.
   * @param r Red component (0-255)
   * @param g Green component (0-255)
   * @param b Blue component (0-255)
   * @param a Alpha component (0-255)
   */
  void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

  /**
   * @brief Set the current drawing color using a Color struct.
   * @param color The color to set.
   */
  void setColor(const nsbaci::types::Color& color);

  /**
   * @brief Get the current drawing color.
   * @return The current color.
   */
  nsbaci::types::Color getCurrentColor() const { return currentColor_; }

  // ============== Position Management ==============

  /**
   * @brief Set the current drawing position.
   * @param x X coordinate
   * @param y Y coordinate
   */
  void setPosition(int32_t x, int32_t y);

  /**
   * @brief Set the current drawing position using a Point.
   * @param point The position to set.
   */
  void setPosition(const nsbaci::types::Point& point);

  /**
   * @brief Get the current drawing position.
   * @return The current position.
   */
  nsbaci::types::Point getCurrentPosition() const { return currentPosition_; }

  // ============== Canvas Operations ==============

  /**
   * @brief Clear the canvas with optional background color.
   */
  void clear();

  /**
   * @brief Clear the canvas with a specific color.
   * @param color The background color.
   */
  void clear(const nsbaci::types::Color& color);

  /**
   * @brief Fill the entire canvas with the current color.
   */
  void fill();

  /**
   * @brief Request a canvas refresh/redraw.
   */
  void refresh();

  /**
   * @brief Set the line thickness for subsequent drawings.
   * @param width Line width in pixels.
   */
  void setLineWidth(int32_t width);

  /**
   * @brief Get the current line width.
   * @return The current line width.
   */
  int32_t getLineWidth() const { return lineWidth_; }

  // ============== Shape Drawing ==============

  /**
   * @brief Draw a circle at the specified position.
   * @param centerX Center X coordinate
   * @param centerY Center Y coordinate
   * @param radius Circle radius
   * @param filled Whether to fill the circle
   */
  void drawCircle(int32_t centerX, int32_t centerY, int32_t radius,
                  bool filled = false);

  /**
   * @brief Draw a rectangle at the specified position.
   * @param x Top-left X coordinate
   * @param y Top-left Y coordinate
   * @param width Rectangle width
   * @param height Rectangle height
   * @param filled Whether to fill the rectangle
   */
  void drawRectangle(int32_t x, int32_t y, int32_t width, int32_t height,
                     bool filled = false);

  /**
   * @brief Draw a triangle with three vertices.
   * @param x1, y1 First vertex
   * @param x2, y2 Second vertex
   * @param x3, y3 Third vertex
   * @param filled Whether to fill the triangle
   */
  void drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3,
                    int32_t y3, bool filled = false);

  /**
   * @brief Draw a line between two points.
   * @param x1, y1 Start point
   * @param x2, y2 End point
   */
  void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

  /**
   * @brief Draw an ellipse at the specified position.
   * @param centerX Center X coordinate
   * @param centerY Center Y coordinate
   * @param radiusX Horizontal radius
   * @param radiusY Vertical radius
   * @param filled Whether to fill the ellipse
   */
  void drawEllipse(int32_t centerX, int32_t centerY, int32_t radiusX,
                   int32_t radiusY, bool filled = false);

  /**
   * @brief Draw a single pixel at the specified position.
   * @param x X coordinate
   * @param y Y coordinate
   */
  void drawPixel(int32_t x, int32_t y);

  /**
   * @brief Draw text at the specified position.
   * @param x X coordinate
   * @param y Y coordinate
   * @param text The text to draw
   * @param fontSize Font size in points
   */
  void drawText(int32_t x, int32_t y, const std::string& text,
                int32_t fontSize = 12);

  /**
   * @brief Draw a generic shape.
   * @param shape The shape to draw.
   */
  void drawShape(const nsbaci::types::Shape& shape);

  // ============== Canvas Configuration ==============

  /**
   * @brief Get the canvas size.
   * @return The canvas size.
   */
  nsbaci::types::Size getCanvasSize() const { return canvasSize_; }

  /**
   * @brief Set the canvas size.
   * @param width Canvas width
   * @param height Canvas height
   */
  void setCanvasSize(int32_t width, int32_t height);

  /**
   * @brief Reset the drawing service to initial state.
   */
  void reset();

  /**
   * @brief Process a drawing command from the runtime.
   *
   * This method is intended to be called from the runtime's drawing callback.
   * It processes the command and emits the appropriate signal.
   *
   * @param command The drawing command to process.
   */
  void processCommand(const nsbaci::types::DrawCommand& command);

 signals:
  /**
   * @brief Emitted when a drawing command should be executed.
   * @param command The drawing command to execute.
   */
  void drawCommandReceived(const nsbaci::types::DrawCommand& command);

  /**
   * @brief Emitted when the canvas should be cleared.
   * @param backgroundColor The color to clear with.
   */
  void clearRequested(const nsbaci::types::Color& backgroundColor);

  /**
   * @brief Emitted when a shape should be drawn.
   * @param drawable The drawable object containing shape and color.
   */
  void drawRequested(const nsbaci::types::Drawable& drawable);

  /**
   * @brief Emitted when the canvas should refresh.
   */
  void refreshRequested();

  /**
   * @brief Emitted when canvas size changes.
   * @param size The new canvas size.
   */
  void canvasSizeChanged(const nsbaci::types::Size& size);

 private:
  nsbaci::types::Color currentColor_{0, 0, 0};      // Default black
  nsbaci::types::Point currentPosition_{0, 0};      // Default origin
  int32_t lineWidth_ = 1;                           // Default line width
  nsbaci::types::Size canvasSize_{800, 600};        // Default canvas size
  nsbaci::types::Color backgroundColor_{255, 255, 255};  // Default white
};

}  // namespace nsbaci::services

#endif  // NSBACI_SERVICES_DRAWINGSERVICE_H