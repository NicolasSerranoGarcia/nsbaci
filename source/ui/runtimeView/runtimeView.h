/**
 * @file runtimeView.h
 * @brief RuntimeView widget declaration for nsbaci.
 *
 * This widget displays runtime information when executing a program:
 * variables, threads, I/O console, and execution controls.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_RUNTIMEVIEW_H
#define NSBACI_RUNTIMEVIEW_H

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "runtimeTypes.h"

namespace nsbaci::ui {

/**
 * @struct ThreadInfo
 * @brief Information about a thread for display.
 */
struct ThreadInfo {
  nsbaci::types::ThreadID id;
  nsbaci::types::ThreadState state;
  size_t pc;
  QString currentInstruction;
};

/**
 * @struct VariableInfo
 * @brief Information about a variable for display.
 */
struct VariableInfo {
  QString name;
  QString type;
  QString value;
  size_t address;
};

/**
 * @class RuntimeView
 * @brief Widget displaying runtime execution state.
 *
 * Shows:
 * - Thread list with states and current instruction
 * - Variables/memory watch panel
 * - I/O console for program input/output
 * - Execution controls (step, run, pause, reset)
 */
class RuntimeView : public QWidget {
  Q_OBJECT

 public:
  explicit RuntimeView(QWidget* parent = nullptr);
  ~RuntimeView() override = default;

 signals:
  // Execution control signals
  void stepRequested();
  void stepThreadRequested(nsbaci::types::ThreadID threadId);
  void runRequested();
  void pauseRequested();
  void resetRequested();
  void stopRequested();

  // I/O signals
  void inputProvided(const QString& input);

 public slots:
  // Update display
  void updateThreads(const std::vector<ThreadInfo>& threads);
  void updateVariables(const std::vector<VariableInfo>& variables);
  void updateCurrentInstruction(const QString& instruction);
  void updateExecutionState(bool running, bool halted);

  // I/O
  void appendOutput(const QString& text);
  void requestInput(const QString& prompt);
  void clearConsole();

  // State
  void onProgramLoaded(const QString& programName);
  void onProgramHalted();

 private slots:
  void onStepClicked();
  void onRunClicked();
  void onPauseClicked();
  void onResetClicked();
  void onStopClicked();
  void onInputSubmitted();
  void onThreadSelected(QTreeWidgetItem* item, int column);

 private:
  void createUI();
  void createToolbar();
  void createThreadPanel();
  void createVariablePanel();
  void createConsolePanel();
  void applyStyleSheet();

  // Toolbar
  QWidget* toolbar = nullptr;
  QToolButton* stepButton = nullptr;
  QToolButton* runButton = nullptr;
  QToolButton* pauseButton = nullptr;
  QToolButton* resetButton = nullptr;
  QToolButton* stopButton = nullptr;
  QLabel* statusLabel = nullptr;

  // Thread panel
  QTreeWidget* threadTree = nullptr;

  // Variable panel
  QTableWidget* variableTable = nullptr;

  // Console panel
  QPlainTextEdit* consoleOutput = nullptr;
  QLineEdit* consoleInput = nullptr;
  QPushButton* inputSubmitButton = nullptr;
  QLabel* inputPromptLabel = nullptr;

  // State
  bool isRunning = false;
  bool isHalted = false;
  bool waitingForInput = false;
  nsbaci::types::ThreadID selectedThread = 0;
};

}  // namespace nsbaci::ui

#endif  // NSBACI_RUNTIMEVIEW_H
