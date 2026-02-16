/**
 * @file drawingWidget.h
 * @brief DrawingWidget class declaration for nsbaci.
 *
 * This widget provides a canvas for drawing shapes and graphics.
 * It receives drawing commands from the DrawingService via signals.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_UI_DRAWINGWIDGET_H
#define NSBACI_UI_DRAWINGWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QPoint>
#include <vector>

#include "drawingTypes.h"

/**
 * @namespace nsbaci::ui
 * @brief User interface namespace for nsbaci.
 */
namespace nsbaci::ui {

/**
 * @class DrawingWidget
 * @brief Qt widget that provides a drawing canvas.
 *
 * The DrawingWidget maintains a pixel buffer (QPixmap) where all drawing
 * operations are performed. It supports various shapes, colors, and
 * provides a persistent canvas that preserves drawings between repaints.
 */
class DrawingWidget : public QWidget {
  Q_OBJECT

 public:
  /**
   * @brief Constructs the drawing widget.
   * @param parent Optional parent widget.
   */
  explicit DrawingWidget(QWidget* parent = nullptr);

  /**
   * @brief Destructor.
   */
  ~DrawingWidget() override = default;

  /**
   * @brief Get the current canvas size.
   * @return The canvas size.
   */
  nsbaci::types::Size getCanvasSize() const;

  /**
   * @brief Get the minimum size hint for the widget.
   * @return The minimum size.
   */
  QSize minimumSizeHint() const override;

  /**
   * @brief Get the size hint for the widget.
   * @return The preferred size.
   */
  QSize sizeHint() const override;

 public slots:
  // ============== Drawing Command Slots ==============

  /**
   * @brief Handle a drawing command.
   * @param command The command to execute.
   */
  void onDrawCommand(const nsbaci::types::DrawCommand& command);

  /**
   * @brief Handle a drawable object.
   * @param drawable The drawable to render.
   */
  void onDrawRequested(const nsbaci::types::Drawable& drawable);

  /**
   * @brief Clear the canvas with a specific color.
   * @param color The background color.
   */
  void onClearRequested(const nsbaci::types::Color& color);

  /**
   * @brief Refresh the canvas display.
   */
  void onRefreshRequested();

  /**
   * @brief Handle canvas size change.
   * @param size The new canvas size.
   */
  void onCanvasSizeChanged(const nsbaci::types::Size& size);

  // ============== Direct Drawing Slots ==============

  /**
   * @brief Set the current drawing color.
   */
  void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

  /**
   * @brief Clear the canvas with the current background color.
   */
  void clear();

  /**
   * @brief Draw a circle.
   */
  void drawCircle(int32_t centerX, int32_t centerY, int32_t radius,
                  bool filled = false);

  /**
   * @brief Draw a rectangle.
   */
  void drawRectangle(int32_t x, int32_t y, int32_t width, int32_t height,
                     bool filled = false);

  /**
   * @brief Draw a triangle.
   */
  void drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                    int32_t x3, int32_t y3, bool filled = false);

  /**
   * @brief Draw a line.
   */
  void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

  /**
   * @brief Draw an ellipse.
   */
  void drawEllipse(int32_t centerX, int32_t centerY, int32_t radiusX,
                   int32_t radiusY, bool filled = false);

  /**
   * @brief Draw a pixel.
   */
  void drawPixel(int32_t x, int32_t y);

  /**
   * @brief Draw text.
   */
  void drawText(int32_t x, int32_t y, const QString& text,
                int32_t fontSize = 12);

  /**
   * @brief Set the line width.
   */
  void setLineWidth(int32_t width);

  /**
   * @brief Reset the widget to initial state.
   */
  void reset();

 protected:
  /**
   * @brief Paint event handler.
   * @param event The paint event.
   */
  void paintEvent(QPaintEvent* event) override;

  /**
   * @brief Resize event handler.
   * @param event The resize event.
   */
  void resizeEvent(QResizeEvent* event) override;

 private:
  /**
   * @brief Draw a shape variant on the canvas.
   * @param shape The shape to draw.
   * @param color The color to use.
   */
  void drawShapeInternal(const nsbaci::types::Shape& shape,
                         const nsbaci::types::Color& color);

  /**
   * @brief Convert a types::Color to QColor.
   */
  static QColor toQColor(const nsbaci::types::Color& color);

  /**
   * @brief Ensure the canvas pixmap is properly sized.
   */
  void ensureCanvas();

  QPixmap canvas_;                        // The drawing buffer
  QColor currentColor_{Qt::black};        // Current drawing color
  QColor backgroundColor_{Qt::white};     // Background color
  int lineWidth_ = 1;                     // Current line width
  nsbaci::types::Size canvasSize_{800, 600};  // Canvas dimensions
};

}  // namespace nsbaci::ui

#endif  // NSBACI_UI_DRAWINGWIDGET_H
