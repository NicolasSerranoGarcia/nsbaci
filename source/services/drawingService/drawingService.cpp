/**
 * @file drawingService.cpp
 * @brief DrawingService class implementation for nsbaci.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "drawingService.h"

namespace nsbaci::services {

using namespace nsbaci::types;

DrawingService::DrawingService(QObject* parent) : QObject(parent) {}

// ============== Color Management ==============

void DrawingService::setColor(uint8_t r, uint8_t g, uint8_t b) {
  currentColor_ = Color(r, g, b);
  emit drawCommandReceived(DrawCommand::setColor(currentColor_));
}

void DrawingService::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  currentColor_ = Color(r, g, b, a);
  emit drawCommandReceived(DrawCommand::setColor(currentColor_));
}

void DrawingService::setColor(const Color& color) {
  currentColor_ = color;
  emit drawCommandReceived(DrawCommand::setColor(currentColor_));
}

// ============== Position Management ==============

void DrawingService::setPosition(int32_t x, int32_t y) {
  currentPosition_ = Point(x, y);
  emit drawCommandReceived(DrawCommand::setPosition(currentPosition_));
}

void DrawingService::setPosition(const Point& point) {
  currentPosition_ = point;
  emit drawCommandReceived(DrawCommand::setPosition(currentPosition_));
}

// ============== Canvas Operations ==============

void DrawingService::clear() {
  emit clearRequested(backgroundColor_);
  emit drawCommandReceived(DrawCommand::clear());
}

void DrawingService::clear(const Color& color) {
  backgroundColor_ = color;
  emit clearRequested(color);
  emit drawCommandReceived(DrawCommand::clear());
}

void DrawingService::fill() {
  emit drawCommandReceived(DrawCommand::fill(currentColor_));
}

void DrawingService::refresh() {
  emit refreshRequested();
  emit drawCommandReceived(DrawCommand::refresh());
}

void DrawingService::setLineWidth(int32_t width) {
  lineWidth_ = width;
  emit drawCommandReceived(DrawCommand::setLineWidth(width));
}

// ============== Shape Drawing ==============

void DrawingService::drawCircle(int32_t centerX, int32_t centerY,
                                 int32_t radius, bool filled) {
  Circle circle(Point(centerX, centerY), radius, filled);
  Drawable drawable(circle, currentColor_);
  emit drawRequested(drawable);
  emit drawCommandReceived(DrawCommand::drawShape(circle, currentColor_));
}

void DrawingService::drawRectangle(int32_t x, int32_t y, int32_t width,
                                    int32_t height, bool filled) {
  Rectangle rect(x, y, width, height, filled);
  Drawable drawable(rect, currentColor_);
  emit drawRequested(drawable);
  emit drawCommandReceived(DrawCommand::drawShape(rect, currentColor_));
}

void DrawingService::drawTriangle(int32_t x1, int32_t y1, int32_t x2,
                                   int32_t y2, int32_t x3, int32_t y3,
                                   bool filled) {
  Triangle tri(Point(x1, y1), Point(x2, y2), Point(x3, y3), filled);
  Drawable drawable(tri, currentColor_);
  emit drawRequested(drawable);
  emit drawCommandReceived(DrawCommand::drawShape(tri, currentColor_));
}

void DrawingService::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
  Line line(Point(x1, y1), Point(x2, y2), lineWidth_);
  Drawable drawable(line, currentColor_);
  emit drawRequested(drawable);
  emit drawCommandReceived(DrawCommand::drawShape(line, currentColor_));
}

void DrawingService::drawEllipse(int32_t centerX, int32_t centerY,
                                  int32_t radiusX, int32_t radiusY,
                                  bool filled) {
  Ellipse ellipse(Point(centerX, centerY), radiusX, radiusY, filled);
  Drawable drawable(ellipse, currentColor_);
  emit drawRequested(drawable);
  emit drawCommandReceived(DrawCommand::drawShape(ellipse, currentColor_));
}

void DrawingService::drawPixel(int32_t x, int32_t y) {
  Pixel pixel(x, y);
  Drawable drawable(pixel, currentColor_);
  emit drawRequested(drawable);
  emit drawCommandReceived(DrawCommand::drawShape(pixel, currentColor_));
}

void DrawingService::drawText(int32_t x, int32_t y, const std::string& text,
                               int32_t fontSize) {
  DrawText drawText(Point(x, y), text, fontSize);
  Drawable drawable(drawText, currentColor_);
  emit drawRequested(drawable);
  emit drawCommandReceived(DrawCommand::drawShape(drawText, currentColor_));
}

void DrawingService::drawShape(const Shape& shape) {
  Drawable drawable(shape, currentColor_);
  emit drawRequested(drawable);
  emit drawCommandReceived(DrawCommand::drawShape(shape, currentColor_));
}

// ============== Canvas Configuration ==============

void DrawingService::setCanvasSize(int32_t width, int32_t height) {
  canvasSize_ = Size(width, height);
  emit canvasSizeChanged(canvasSize_);
}

void DrawingService::reset() {
  currentColor_ = Color(0, 0, 0);
  currentPosition_ = Point(0, 0);
  lineWidth_ = 1;
  backgroundColor_ = Color(255, 255, 255);
  clear();
}

void DrawingService::processCommand(const DrawCommand& command) {
  // Simply emit the command for the widget to process
  emit drawCommandReceived(command);
}

}  // namespace nsbaci::services
