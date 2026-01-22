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
#include <QStackedWidget>
#include <QToolButton>

#include "codeeditor.h"
#include "errorDialogFactory.h"
#include "runtimeView.h"
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

  // Runtime control signals
  void stepRequested();
  void stepThreadRequested(nsbaci::types::ThreadID threadId);
  void runContinueRequested();
  void pauseRequested();
  void resetRequested();
  void stopRequested();
  void inputProvided(const QString& input);

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

  // Runtime slots
  void onRunStarted(const QString& programName);
  void onRuntimeStateChanged(bool running, bool halted);
  void onThreadsUpdated(const std::vector<nsbaci::ui::ThreadInfo>& threads);
  void onVariablesUpdated(
      const std::vector<nsbaci::ui::VariableInfo>& variables);
  void onOutputReceived(const QString& output);
  void onInputRequested(const QString& prompt);

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

  // Runtime view
  void onStopRuntime();

 private:
  // Stacked widget for switching views
  QStackedWidget* centralStack = nullptr;

  // Editor view container
  QWidget* editorView = nullptr;

  // File info bar
  QFrame* fileInfoBar = nullptr;
  QLabel* fileNameLabel = nullptr;
  QLabel* fileModifiedIndicator = nullptr;
  QLabel* compileStatusIndicator = nullptr;

  // Central widget
  CodeEditor* codeEditor = nullptr;

  // Sidebar
  QFrame* sideBar = nullptr;
  QToolButton* compileButton = nullptr;
  QToolButton* runButton = nullptr;

  // Runtime view
  nsbaci::ui::RuntimeView* runtimeView = nullptr;

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
  bool isCompiled =
      false;  // True when program is compiled and code hasn't changed

 private:
  void createCentralWidget();
  void createEditorView();
  void createRuntimeView();
  void createMenuBar();
  void createStatusBar();
  void setupShortcuts();
  void applyStyleSheet();

  void switchToEditor();
  void switchToRuntime();
};
#endif  // MAINWINDOW_H
