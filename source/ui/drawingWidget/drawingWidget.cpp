/**
 * @file drawingWidget.cpp
 * @brief DrawingWidget class implementation for nsbaci.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "drawingWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPolygon>
#include <cmath>

namespace nsbaci::ui {

using namespace nsbaci::types;

DrawingWidget::DrawingWidget(QWidget* parent) : QWidget(parent) {
  setMinimumSize(400, 300);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setAutoFillBackground(true);
  setWindowIcon(QIcon(":/assets/nsbaci.ico"));
  // Initialize the canvas
  ensureCanvas();
  clear();
}

nsbaci::types::Size DrawingWidget::getCanvasSize() const {
  return canvasSize_;
}

QSize DrawingWidget::minimumSizeHint() const {
  return QSize(400, 300);
}

QSize DrawingWidget::sizeHint() const {
  return QSize(canvasSize_.width, canvasSize_.height);
}

// ============== Drawing Command Slots ==============

void DrawingWidget::onDrawCommand(const DrawCommand& command) {
  switch (command.type) {
    case DrawCommandType::Clear:
      // Use the color from the command to clear the canvas
      backgroundColor_ = toQColor(command.color);
      clear();
      break;
    case DrawCommandType::SetColor:
      setColor(command.color.r, command.color.g, command.color.b,
               command.color.a);
      break;
    case DrawCommandType::SetPosition:
      // Position is handled per-shape
      break;
    case DrawCommandType::DrawShape:
      drawShapeInternal(command.shape, command.color);
      break;
    case DrawCommandType::Fill: {
      QPainter painter(&canvas_);
      painter.fillRect(canvas_.rect(), toQColor(command.color));
      update();
      break;
    }
    case DrawCommandType::SetLineWidth:
      setLineWidth(command.lineWidth);
      break;
    case DrawCommandType::Refresh:
      update();
      break;
  }
}

void DrawingWidget::onDrawRequested(const Drawable& drawable) {
  if (!drawable.visible) return;
  drawShapeInternal(drawable.shape, drawable.color);
}

void DrawingWidget::onClearRequested(const Color& color) {
  backgroundColor_ = toQColor(color);
  clear();
}

void DrawingWidget::onRefreshRequested() {
  update();
}

void DrawingWidget::onCanvasSizeChanged(const Size& size) {
  canvasSize_ = size;
  ensureCanvas();
  clear();
  updateGeometry();
}

// ============== Direct Drawing Slots ==============

void DrawingWidget::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  currentColor_ = QColor(r, g, b, a);
}

void DrawingWidget::clear() {
  ensureCanvas();
  canvas_.fill(backgroundColor_);
  update();
}

void DrawingWidget::drawCircle(int32_t centerX, int32_t centerY, int32_t radius,
                                bool filled) {
  ensureCanvas();
  QPainter painter(&canvas_);
  painter.setRenderHint(QPainter::Antialiasing);

  QPen pen(currentColor_);
  pen.setWidth(lineWidth_);
  painter.setPen(pen);

  if (filled) {
    painter.setBrush(QBrush(currentColor_));
  } else {
    painter.setBrush(Qt::NoBrush);
  }

  painter.drawEllipse(QPoint(centerX, centerY), radius, radius);
  update();
}

void DrawingWidget::drawRectangle(int32_t x, int32_t y, int32_t width,
                                   int32_t height, bool filled) {
  ensureCanvas();
  QPainter painter(&canvas_);
  painter.setRenderHint(QPainter::Antialiasing);

  QPen pen(currentColor_);
  pen.setWidth(lineWidth_);
  painter.setPen(pen);

  if (filled) {
    painter.setBrush(QBrush(currentColor_));
  } else {
    painter.setBrush(Qt::NoBrush);
  }

  painter.drawRect(x, y, width, height);
  update();
}

void DrawingWidget::drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                  int32_t x3, int32_t y3, bool filled) {
  ensureCanvas();
  QPainter painter(&canvas_);
  painter.setRenderHint(QPainter::Antialiasing);

  QPen pen(currentColor_);
  pen.setWidth(lineWidth_);
  painter.setPen(pen);

  if (filled) {
    painter.setBrush(QBrush(currentColor_));
  } else {
    painter.setBrush(Qt::NoBrush);
  }

  QPolygon triangle;
  triangle << QPoint(x1, y1) << QPoint(x2, y2) << QPoint(x3, y3);
  painter.drawPolygon(triangle);
  update();
}

void DrawingWidget::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
  ensureCanvas();
  QPainter painter(&canvas_);
  painter.setRenderHint(QPainter::Antialiasing);

  QPen pen(currentColor_);
  pen.setWidth(lineWidth_);
  painter.setPen(pen);

  painter.drawLine(x1, y1, x2, y2);
  update();
}

void DrawingWidget::drawEllipse(int32_t centerX, int32_t centerY,
                                 int32_t radiusX, int32_t radiusY,
                                 bool filled) {
  ensureCanvas();
  QPainter painter(&canvas_);
  painter.setRenderHint(QPainter::Antialiasing);

  QPen pen(currentColor_);
  pen.setWidth(lineWidth_);
  painter.setPen(pen);

  if (filled) {
    painter.setBrush(QBrush(currentColor_));
  } else {
    painter.setBrush(Qt::NoBrush);
  }

  painter.drawEllipse(QPoint(centerX, centerY), radiusX, radiusY);
  update();
}

void DrawingWidget::drawPixel(int32_t x, int32_t y) {
  ensureCanvas();
  QPainter painter(&canvas_);
  painter.setPen(currentColor_);
  painter.drawPoint(x, y);
  update();
}

void DrawingWidget::drawText(int32_t x, int32_t y, const QString& text,
                              int32_t fontSize) {
  ensureCanvas();
  QPainter painter(&canvas_);
  painter.setRenderHint(QPainter::TextAntialiasing);

  QFont font = painter.font();
  font.setPointSize(fontSize);
  painter.setFont(font);
  painter.setPen(currentColor_);

  painter.drawText(x, y, text);
  update();
}

void DrawingWidget::setLineWidth(int32_t width) {
  lineWidth_ = width;
}

void DrawingWidget::reset() {
  currentColor_ = Qt::black;
  backgroundColor_ = Qt::white;
  lineWidth_ = 1;
  clear();
}

// ============== Protected Methods ==============

void DrawingWidget::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  QPainter painter(this);

  // The widget maintains canvas aspect ratio, so we can fill the entire widget
  painter.drawPixmap(rect(), canvas_);
}

void DrawingWidget::resizeEvent(QResizeEvent* event) {
  // Enforce canvas aspect ratio to ensure the canvas always fills the widget
  // When user resizes in any direction, scale both axes proportionally
  QSize oldSize = event->oldSize();
  QSize newSize = event->size();
  double aspectRatio =
      static_cast<double>(canvasSize_.width) / canvasSize_.height;

  // Determine which axis changed more (or if both changed)
  int deltaWidth = newSize.width() - oldSize.width();
  int deltaHeight = newSize.height() - oldSize.height();

  int targetWidth, targetHeight;

  // Use the axis with the larger change to drive the resize
  if (std::abs(deltaWidth) >= std::abs(deltaHeight)) {
    // Width changed more - calculate height from width
    targetWidth = newSize.width();
    targetHeight = static_cast<int>(targetWidth / aspectRatio);
  } else {
    // Height changed more - calculate width from height
    targetHeight = newSize.height();
    targetWidth = static_cast<int>(targetHeight * aspectRatio);
  }

  // Ensure minimum size
  if (targetWidth < 400) {
    targetWidth = 400;
    targetHeight = static_cast<int>(targetWidth / aspectRatio);
  }
  if (targetHeight < 300) {
    targetHeight = 300;
    targetWidth = static_cast<int>(targetHeight * aspectRatio);
  }

  // Only resize if needed to avoid infinite recursion
  if (targetWidth != newSize.width() || targetHeight != newSize.height()) {
    resize(targetWidth, targetHeight);
  }

  QWidget::resizeEvent(event);
}

// ============== Private Methods ==============

void DrawingWidget::drawShapeInternal(const Shape& shape, const Color& color) {
  QColor qcolor = toQColor(color);
  QColor savedColor = currentColor_;
  currentColor_ = qcolor;

  std::visit(
      [this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Circle>) {
          drawCircle(arg.center.x, arg.center.y, arg.radius, arg.filled);
        } else if constexpr (std::is_same_v<T, Rectangle>) {
          drawRectangle(arg.position.x, arg.position.y, arg.size.width,
                        arg.size.height, arg.filled);
        } else if constexpr (std::is_same_v<T, Triangle>) {
          drawTriangle(arg.p1.x, arg.p1.y, arg.p2.x, arg.p2.y, arg.p3.x,
                       arg.p3.y, arg.filled);
        } else if constexpr (std::is_same_v<T, Line>) {
          int savedWidth = lineWidth_;
          lineWidth_ = arg.thickness;
          drawLine(arg.start.x, arg.start.y, arg.end.x, arg.end.y);
          lineWidth_ = savedWidth;
        } else if constexpr (std::is_same_v<T, Ellipse>) {
          drawEllipse(arg.center.x, arg.center.y, arg.radiusX, arg.radiusY,
                      arg.filled);
        } else if constexpr (std::is_same_v<T, Pixel>) {
          drawPixel(arg.position.x, arg.position.y);
        } else if constexpr (std::is_same_v<T, DrawText>) {
          drawText(arg.position.x, arg.position.y,
                   QString::fromStdString(arg.content), arg.fontSize);
        }
      },
      shape);

  currentColor_ = savedColor;
}

QColor DrawingWidget::toQColor(const Color& color) {
  return QColor(color.r, color.g, color.b, color.a);
}

void DrawingWidget::ensureCanvas() {
  if (canvas_.isNull() || canvas_.width() != canvasSize_.width ||
      canvas_.height() != canvasSize_.height) {
    QPixmap newCanvas(canvasSize_.width, canvasSize_.height);
    newCanvas.fill(backgroundColor_);

    // Copy old content if exists
    if (!canvas_.isNull()) {
      QPainter painter(&newCanvas);
      painter.drawPixmap(0, 0, canvas_);
    }

    canvas_ = newCanvas;
  }
}

}  // namespace nsbaci::ui
