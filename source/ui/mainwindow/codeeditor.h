/**
 * @file codeeditor.h
 * @brief CodeEditor class with line numbers for nsbaci.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QWidget>

class LineNumberArea;

class CodeEditor : public QPlainTextEdit {
  Q_OBJECT

 public:
  explicit CodeEditor(QWidget* parent = nullptr);

  void lineNumberAreaPaintEvent(QPaintEvent* event);
  int lineNumberAreaWidth();

  /** @brief Show or hide the line number gutter. */
  void setLineNumbersVisible(bool visible);
  bool lineNumbersVisible() const;

  /** @brief Set whether to use light theme colors for line numbers and
   * highlights. */
  void setLightTheme(bool light);

 protected:
  void resizeEvent(QResizeEvent* event) override;

 private slots:
  void updateLineNumberAreaWidth(int newBlockCount);
  void highlightCurrentLine();
  void updateLineNumberArea(const QRect& rect, int dy);

 private:
  LineNumberArea* lineNumberArea;
  bool showLineNumbers = true;
  bool lightTheme = false;
};

class LineNumberArea : public QWidget {
 public:
  explicit LineNumberArea(CodeEditor* editor)
      : QWidget(editor), codeEditor(editor) {}

  QSize sizeHint() const override {
    return QSize(codeEditor->lineNumberAreaWidth(), 0);
  }

 protected:
  void paintEvent(QPaintEvent* event) override {
    codeEditor->lineNumberAreaPaintEvent(event);
  }

 private:
  CodeEditor* codeEditor;
};

#endif  // CODEEDITOR_H
