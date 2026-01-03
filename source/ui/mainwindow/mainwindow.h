/**
 * @file mainwindow.h
 * @brief Main window class declaration for nsbaci.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QFrame>
#include <QLabel>
#include <QMainWindow>
#include <QShortcut>
#include <QToolButton>

#include "codeeditor.h"
#include "errorDialogFactory.h"
#include "uiError.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override = default;

 signals:
  void saveRequested(const QString& filePath, const QString& contents);
  void openRequested(const QString& filePath);
  void compileRequested(const QString& contents);
  void runRequested();

 public slots:
  void setEditorContents(const QString& contents);
  void setStatusMessage(const QString& message);
  void setCurrentFile(const QString& fileName, bool modified = false);

  // Controller response slots
  void onSaveSucceeded();
  void onSaveFailed(std::vector<nsbaci::UIError> errors);
  void onLoadSucceeded(const QString& contents);
  void onLoadFailed(std::vector<nsbaci::UIError> errors);
  void onCompileSucceeded();
  void onCompileFailed(std::vector<nsbaci::UIError> errors);

 private slots:
  // File menu
  void onNew();
  void onSave();
  void onSaveAs();
  void onOpen();
  void onExit();
  void onCompile();
  void onRun();

  // Edit menu
  void onUndo();
  void onRedo();
  void onCut();
  void onCopy();
  void onPaste();
  void onSelectAll();

  // View menu
  void onToggleSidebar();
  void onToggleFullscreen();

  // Help menu
  void onAbout();

  // Editor
  void onTextChanged();

 private:
  // File info bar
  QFrame* fileInfoBar = nullptr;
  QLabel* fileNameLabel = nullptr;
  QLabel* fileModifiedIndicator = nullptr;

  // Central widget
  CodeEditor* codeEditor = nullptr;

  // Sidebar
  QFrame* sideBar = nullptr;
  QToolButton* compileButton = nullptr;
  QToolButton* runButton = nullptr;

  // File actions
  QAction* actionNew = nullptr;
  QAction* actionSave = nullptr;
  QAction* actionSaveAs = nullptr;
  QAction* actionOpen = nullptr;
  QAction* actionExit = nullptr;

  // Edit actions
  QAction* actionUndo = nullptr;
  QAction* actionRedo = nullptr;
  QAction* actionCut = nullptr;
  QAction* actionCopy = nullptr;
  QAction* actionPaste = nullptr;
  QAction* actionSelectAll = nullptr;

  // View actions
  QAction* actionToggleSidebar = nullptr;
  QAction* actionFullscreen = nullptr;

  // Build actions
  QAction* actionCompile = nullptr;
  QAction* actionRun = nullptr;

  // Help actions
  QAction* actionAbout = nullptr;

  // State
  QString currentFileName;  // Just the filename for display
  QString currentFilePath;  // Full path for saving
  bool isModified = false;
  bool hasName = false;

 private:
  void createCentralWidget();
  void createMenuBar();
  void createStatusBar();
  void setupShortcuts();
  void applyStyleSheet();
};
#endif  // MAINWINDOW_H
