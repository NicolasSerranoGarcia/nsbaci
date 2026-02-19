/**
 * @file drawingTypes.h
 * @brief Type definitions for drawing-related operations.
 *
 * This header provides type definitions for the drawing system including
 * colors, positions, shapes, and drawable interfaces.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_TYPES_DRAWINGTYPES_H
#define NSBACI_TYPES_DRAWINGTYPES_H

#include <cstdint>
#include <memory>
#include <string>
#include <variant>

/**
 * @namespace nsbaci::types
 * @brief Type definitions namespace for nsbaci.
 */
namespace nsbaci::types {

/**
 * @struct Color
 * @brief RGB color representation with values from 0-255.
 */
struct Color {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  uint8_t a = 255;  // Alpha channel for transparency

  constexpr Color() = default;
  constexpr Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
      : r(red), g(green), b(blue), a(alpha) {}

  constexpr bool operator==(const Color& other) const {
    return r == other.r && g == other.g && b == other.b && a == other.a;
  }
};

// ============== Predefined Colors ==============
namespace Colors {
inline constexpr Color BLACK{0, 0, 0};
inline constexpr Color WHITE{255, 255, 255};
inline constexpr Color RED{255, 0, 0};
inline constexpr Color GREEN{0, 255, 0};
inline constexpr Color BLUE{0, 0, 255};
inline constexpr Color YELLOW{255, 255, 0};
inline constexpr Color CYAN{0, 255, 255};
inline constexpr Color MAGENTA{255, 0, 255};
inline constexpr Color ORANGE{255, 165, 0};
inline constexpr Color PINK{255, 192, 203};
inline constexpr Color PURPLE{128, 0, 128};
inline constexpr Color GRAY{128, 128, 128};
inline constexpr Color LIGHT_GRAY{192, 192, 192};
inline constexpr Color DARK_GRAY{64, 64, 64};
inline constexpr Color BROWN{139, 69, 19};
}  // namespace Colors

/**
 * @struct Point
 * @brief 2D point/position representation.
 */
struct Point {
  int32_t x = 0;
  int32_t y = 0;

  Point() = default;
  Point(int32_t px, int32_t py) : x(px), y(py) {}

  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }

  Point operator+(const Point& other) const {
    return Point(x + other.x, y + other.y);
  }

  Point operator-(const Point& other) const {
    return Point(x - other.x, y - other.y);
  }
};

/**
 * @struct Size
 * @brief 2D size representation.
 */
struct Size {
  int32_t width = 0;
  int32_t height = 0;

  Size() = default;
  Size(int32_t w, int32_t h) : width(w), height(h) {}
};

// ============== Shape Definitions ==============

/**
 * @struct Circle
 * @brief Circle shape with center and radius.
 */
struct Circle {
  Point center;
  int32_t radius = 0;
  bool filled = false;

  Circle() = default;
  Circle(Point c, int32_t r, bool fill = false)
      : center(c), radius(r), filled(fill) {}
};

/**
 * @struct Rectangle
 * @brief Rectangle shape with position and size.
 */
struct Rectangle {
  Point position;  // Top-left corner
  Size size;
  bool filled = false;

  Rectangle() = default;
  Rectangle(Point pos, Size s, bool fill = false)
      : position(pos), size(s), filled(fill) {}
  Rectangle(int32_t x, int32_t y, int32_t w, int32_t h, bool fill = false)
      : position(x, y), size(w, h), filled(fill) {}
};

/**
 * @struct Triangle
 * @brief Triangle shape defined by three vertices.
 */
struct Triangle {
  Point p1, p2, p3;
  bool filled = false;

  Triangle() = default;
  Triangle(Point a, Point b, Point c, bool fill = false)
      : p1(a), p2(b), p3(c), filled(fill) {}
};

/**
 * @struct Line
 * @brief Line segment from start to end point.
 */
struct Line {
  Point start;
  Point end;
  int32_t thickness = 1;

  Line() = default;
  Line(Point s, Point e, int32_t t = 1) : start(s), end(e), thickness(t) {}
};

/**
 * @struct Ellipse
 * @brief Ellipse shape with center and radii.
 */
struct Ellipse {
  Point center;
  int32_t radiusX = 0;
  int32_t radiusY = 0;
  bool filled = false;

  Ellipse() = default;
  Ellipse(Point c, int32_t rx, int32_t ry, bool fill = false)
      : center(c), radiusX(rx), radiusY(ry), filled(fill) {}
};

/**
 * @struct Pixel
 * @brief Single pixel at a position.
 */
struct Pixel {
  Point position;

  Pixel() = default;
  explicit Pixel(Point p) : position(p) {}
  Pixel(int32_t x, int32_t y) : position(x, y) {}
};

/**
 * @struct Text
 * @brief Text to be drawn at a position.
 */
struct DrawText {
  Point position;
  std::string content;
  int32_t fontSize = 12;

  DrawText() = default;
  DrawText(Point p, std::string text, int32_t size = 12)
      : position(p), content(std::move(text)), fontSize(size) {}
};

/**
 * @brief Variant type for all drawable shapes.
 */
using Shape = std::variant<Circle, Rectangle, Triangle, Line, Ellipse, Pixel, DrawText>;

/**
 * @struct Drawable
 * @brief Complete drawable object with shape, color, and visibility.
 */
struct Drawable {
  Shape shape;
  Color color;
  bool visible = true;
  int32_t zIndex = 0;  // For layering

  Drawable() = default;
  Drawable(Shape s, Color c, bool vis = true, int32_t z = 0)
      : shape(std::move(s)), color(c), visible(vis), zIndex(z) {}
};

// ============== Drawing Commands ==============

/**
 * @enum DrawCommandType
 * @brief Types of drawing commands that can be executed.
 */
enum class DrawCommandType {
  Clear,          // Clear the canvas
  SetColor,       // Set current drawing color
  SetPosition,    // Set current drawing position
  DrawShape,      // Draw a shape
  Fill,           // Fill the canvas with current color
  SetLineWidth,   // Set line thickness
  Refresh         // Force refresh/redraw
};

/**
 * @struct DrawCommand
 * @brief Represents a single drawing command to be executed.
 */
struct DrawCommand {
  DrawCommandType type;
  Color color;           // For SetColor
  Point position;        // For SetPosition
  Shape shape;           // For DrawShape
  int32_t lineWidth = 1; // For SetLineWidth

  // Factory methods for convenience
  static DrawCommand clear() {
    DrawCommand cmd;
    cmd.type = DrawCommandType::Clear;
    cmd.color = Color(255, 255, 255);  // Default white
    return cmd;
  }

  static DrawCommand clearWithColor(Color c) {
    DrawCommand cmd;
    cmd.type = DrawCommandType::Clear;
    cmd.color = c;
    return cmd;
  }

  static DrawCommand setColor(Color c) {
    DrawCommand cmd;
    cmd.type = DrawCommandType::SetColor;
    cmd.color = c;
    return cmd;
  }

  static DrawCommand setPosition(Point p) {
    DrawCommand cmd;
    cmd.type = DrawCommandType::SetPosition;
    cmd.position = p;
    return cmd;
  }

  static DrawCommand drawShape(Shape s, Color c) {
    DrawCommand cmd;
    cmd.type = DrawCommandType::DrawShape;
    cmd.shape = std::move(s);
    cmd.color = c;
    return cmd;
  }

  static DrawCommand fill(Color c) {
    DrawCommand cmd;
    cmd.type = DrawCommandType::Fill;
    cmd.color = c;
    return cmd;
  }

  static DrawCommand setLineWidth(int32_t width) {
    DrawCommand cmd;
    cmd.type = DrawCommandType::SetLineWidth;
    cmd.lineWidth = width;
    return cmd;
  }

  static DrawCommand refresh() {
    DrawCommand cmd;
    cmd.type = DrawCommandType::Refresh;
    return cmd;
  }
};

// ============== Canvas Configuration ==============

/**
 * @struct CanvasConfig
 * @brief Configuration for the drawing canvas.
 */
struct CanvasConfig {
  Size size{800, 600};           // Default canvas size
  Color backgroundColor{255, 255, 255};  // Default white background
  std::string title = "NSBACI Canvas";
};

// ============== Predefined Positions ==============
// These are resolved at runtime based on canvas size
enum class StandardPosition {
  TopLeft,
  TopCenter,
  TopRight,
  CenterLeft,
  Center,
  CenterRight,
  BottomLeft,
  BottomCenter,
  BottomRight
};

/**
 * @brief Resolve a standard position to actual coordinates.
 * @param pos The standard position enum value.
 * @param canvasSize The size of the canvas.
 * @return The resolved Point coordinates.
 */
inline Point resolvePosition(StandardPosition pos, Size canvasSize) {
  switch (pos) {
    case StandardPosition::TopLeft:
      return Point(0, 0);
    case StandardPosition::TopCenter:
      return Point(canvasSize.width / 2, 0);
    case StandardPosition::TopRight:
      return Point(canvasSize.width, 0);
    case StandardPosition::CenterLeft:
      return Point(0, canvasSize.height / 2);
    case StandardPosition::Center:
      return Point(canvasSize.width / 2, canvasSize.height / 2);
    case StandardPosition::CenterRight:
      return Point(canvasSize.width, canvasSize.height / 2);
    case StandardPosition::BottomLeft:
      return Point(0, canvasSize.height);
    case StandardPosition::BottomCenter:
      return Point(canvasSize.width / 2, canvasSize.height);
    case StandardPosition::BottomRight:
      return Point(canvasSize.width, canvasSize.height);
    default:
      return Point(0, 0);
  }
}

}  // namespace nsbaci::types

#endif  // NSBACI_TYPES_DRAWINGTYPES_H
