/**
 * @file runtimeView.cpp
 * @brief Implementation of the RuntimeView widget.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "runtimeView.h"

#include <QFrame>
#include <QGroupBox>
#include <QStyle>

namespace nsbaci::ui {

RuntimeView::RuntimeView(QWidget* parent) : QWidget(parent) {
  createUI();
  applyStyleSheet();
}

void RuntimeView::createUI() {
  auto* mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // Toolbar at top
  createToolbar();
  mainLayout->addWidget(toolbar);

  // Main content area with splitters
  auto* contentSplitter = new QSplitter(Qt::Horizontal, this);
  contentSplitter->setObjectName("contentSplitter");

  // Left side: threads and variables (vertical splitter)
  auto* leftSplitter = new QSplitter(Qt::Vertical);
  leftSplitter->setObjectName("leftSplitter");

  // Thread panel
  auto* threadContainer = new QWidget();
  auto* threadLayout = new QVBoxLayout(threadContainer);
  threadLayout->setContentsMargins(8, 8, 8, 4);
  auto* threadLabel = new QLabel("Threads");
  threadLabel->setObjectName("panelLabel");
  threadLayout->addWidget(threadLabel);
  createThreadPanel();
  threadLayout->addWidget(threadTree);
  leftSplitter->addWidget(threadContainer);

  // Variable panel
  auto* varContainer = new QWidget();
  auto* varLayout = new QVBoxLayout(varContainer);
  varLayout->setContentsMargins(8, 4, 8, 8);
  auto* varLabel = new QLabel("Variables");
  varLabel->setObjectName("panelLabel");
  varLayout->addWidget(varLabel);
  createVariablePanel();
  varLayout->addWidget(variableTable);
  leftSplitter->addWidget(varContainer);

  leftSplitter->setSizes({200, 200});
  contentSplitter->addWidget(leftSplitter);

  // Right side: console
  auto* consoleContainer = new QWidget();
  auto* consoleLayout = new QVBoxLayout(consoleContainer);
  consoleLayout->setContentsMargins(8, 8, 8, 8);
  auto* consoleLabel = new QLabel("Console");
  consoleLabel->setObjectName("panelLabel");
  consoleLayout->addWidget(consoleLabel);
  createConsolePanel();
  consoleLayout->addWidget(consoleOutput, 1);

  // Input area
  auto* inputArea = new QWidget();
  auto* inputLayout = new QHBoxLayout(inputArea);
  inputLayout->setContentsMargins(0, 4, 0, 0);
  inputPromptLabel = new QLabel(">");
  inputPromptLabel->setObjectName("inputPrompt");
  inputLayout->addWidget(inputPromptLabel);
  inputLayout->addWidget(consoleInput, 1);
  inputLayout->addWidget(inputSubmitButton);
  consoleLayout->addWidget(inputArea);

  contentSplitter->addWidget(consoleContainer);
  contentSplitter->setSizes({350, 550});

  mainLayout->addWidget(contentSplitter, 1);
}

void RuntimeView::createToolbar() {
  toolbar = new QWidget(this);
  toolbar->setObjectName("runtimeToolbar");
  auto* layout = new QHBoxLayout(toolbar);
  layout->setContentsMargins(8, 6, 8, 6);
  layout->setSpacing(6);

  QStyle* style = this->style();

  // Step button
  stepButton = new QToolButton();
  stepButton->setObjectName("stepButton");
  stepButton->setText("Step");
  stepButton->setIcon(style->standardIcon(QStyle::SP_MediaSkipForward));
  stepButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  stepButton->setToolTip("Execute one instruction (F10)");
  connect(stepButton, &QToolButton::clicked, this, &RuntimeView::onStepClicked);
  layout->addWidget(stepButton);

  // Run button
  runButton = new QToolButton();
  runButton->setObjectName("runButton");
  runButton->setText("Run");
  runButton->setIcon(style->standardIcon(QStyle::SP_MediaPlay));
  runButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  runButton->setToolTip("Run until halted or paused (F5)");
  connect(runButton, &QToolButton::clicked, this, &RuntimeView::onRunClicked);
  layout->addWidget(runButton);

  // Pause button
  pauseButton = new QToolButton();
  pauseButton->setObjectName("pauseButton");
  pauseButton->setText("Pause");
  pauseButton->setIcon(style->standardIcon(QStyle::SP_MediaPause));
  pauseButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  pauseButton->setToolTip("Pause execution");
  pauseButton->setEnabled(false);
  connect(pauseButton, &QToolButton::clicked, this,
          &RuntimeView::onPauseClicked);
  layout->addWidget(pauseButton);

  layout->addSpacing(12);

  // Reset button
  resetButton = new QToolButton();
  resetButton->setObjectName("resetButton");
  resetButton->setText("Reset");
  resetButton->setIcon(style->standardIcon(QStyle::SP_BrowserReload));
  resetButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  resetButton->setToolTip("Reset to beginning");
  connect(resetButton, &QToolButton::clicked, this,
          &RuntimeView::onResetClicked);
  layout->addWidget(resetButton);

  // Stop button
  stopButton = new QToolButton();
  stopButton->setObjectName("stopButton");
  stopButton->setText("Stop");
  stopButton->setIcon(style->standardIcon(QStyle::SP_BrowserStop));
  stopButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  stopButton->setToolTip("Stop and return to editor");
  connect(stopButton, &QToolButton::clicked, this, &RuntimeView::onStopClicked);
  layout->addWidget(stopButton);

  layout->addStretch();

  // Status label
  statusLabel = new QLabel("Ready");
  statusLabel->setObjectName("runtimeStatus");
  layout->addWidget(statusLabel);
}

void RuntimeView::createThreadPanel() {
  threadTree = new QTreeWidget(this);
  threadTree->setObjectName("threadTree");
  threadTree->setHeaderLabels({"ID", "State", "PC", "Instruction"});
  threadTree->setRootIsDecorated(false);
  threadTree->setAlternatingRowColors(true);
  threadTree->setSelectionMode(QAbstractItemView::SingleSelection);

  // Set column widths
  threadTree->header()->setStretchLastSection(true);
  threadTree->setColumnWidth(0, 40);
  threadTree->setColumnWidth(1, 70);
  threadTree->setColumnWidth(2, 50);

  connect(threadTree, &QTreeWidget::itemClicked, this,
          &RuntimeView::onThreadSelected);
}

void RuntimeView::createVariablePanel() {
  variableTable = new QTableWidget(this);
  variableTable->setObjectName("variableTable");
  variableTable->setColumnCount(4);
  variableTable->setHorizontalHeaderLabels(
      {"Name", "Type", "Value", "Address"});
  variableTable->setAlternatingRowColors(true);
  variableTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  variableTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

  // Set column widths
  variableTable->horizontalHeader()->setStretchLastSection(false);
  variableTable->setColumnWidth(0, 100);
  variableTable->setColumnWidth(1, 60);
  variableTable->setColumnWidth(2, 80);
  variableTable->setColumnWidth(3, 60);
  variableTable->horizontalHeader()->setSectionResizeMode(0,
                                                          QHeaderView::Stretch);
}

void RuntimeView::createConsolePanel() {
  consoleOutput = new QPlainTextEdit(this);
  consoleOutput->setObjectName("consoleOutput");
  consoleOutput->setReadOnly(true);
  consoleOutput->setPlaceholderText("Program output will appear here...");

  consoleInput = new QLineEdit(this);
  consoleInput->setObjectName("consoleInput");
  consoleInput->setPlaceholderText("Enter input here...");
  consoleInput->setEnabled(false);
  connect(consoleInput, &QLineEdit::returnPressed, this,
          &RuntimeView::onInputSubmitted);

  inputSubmitButton = new QPushButton("Send", this);
  inputSubmitButton->setObjectName("inputSubmitButton");
  inputSubmitButton->setEnabled(false);
  connect(inputSubmitButton, &QPushButton::clicked, this,
          &RuntimeView::onInputSubmitted);
}

void RuntimeView::applyStyleSheet() {
  QString styleSheet = R"(
    RuntimeView {
      background-color: #1a1a1a;
    }

    /* Toolbar */
    QWidget#runtimeToolbar {
      background-color: #242424;
      border-bottom: 1px solid #333333;
    }

    QToolButton {
      background-color: #2a2a2a;
      color: #d0d0d0;
      border: 1px solid #353535;
      border-radius: 6px;
      padding: 6px 12px;
      font-size: 12px;
      font-weight: 500;
    }
    QToolButton:hover {
      background-color: #353535;
      border-color: #454545;
    }
    QToolButton:pressed {
      background-color: #404040;
    }
    QToolButton:disabled {
      background-color: #252525;
      color: #606060;
      border-color: #303030;
    }
    QToolButton#runButton {
      background-color: #1e4620;
      border-color: #2d6830;
    }
    QToolButton#runButton:hover {
      background-color: #2d6830;
    }
    QToolButton#stopButton {
      background-color: #4a2020;
      border-color: #6a3030;
    }
    QToolButton#stopButton:hover {
      background-color: #6a3030;
    }

    QLabel#runtimeStatus {
      color: #909090;
      font-size: 12px;
      padding-right: 8px;
    }

    /* Panel labels */
    QLabel#panelLabel {
      color: #b0b0b0;
      font-size: 12px;
      font-weight: 600;
      padding-bottom: 4px;
    }

    /* Splitters */
    QSplitter::handle {
      background-color: #333333;
    }
    QSplitter::handle:horizontal {
      width: 2px;
    }
    QSplitter::handle:vertical {
      height: 2px;
    }

    /* Thread tree */
    QTreeWidget#threadTree {
      background-color: #1e1e1e;
      color: #d0d0d0;
      border: 1px solid #333333;
      border-radius: 6px;
      font-family: "JetBrains Mono", "Consolas", monospace;
      font-size: 11px;
    }
    QTreeWidget#threadTree::item {
      padding: 4px;
    }
    QTreeWidget#threadTree::item:selected {
      background-color: #2a4a6a;
    }
    QTreeWidget#threadTree::item:alternate {
      background-color: #222222;
    }
    QHeaderView::section {
      background-color: #252525;
      color: #a0a0a0;
      padding: 6px;
      border: none;
      border-bottom: 1px solid #333333;
      font-size: 11px;
    }

    /* Variable table */
    QTableWidget#variableTable {
      background-color: #1e1e1e;
      color: #d0d0d0;
      border: 1px solid #333333;
      border-radius: 6px;
      font-family: "JetBrains Mono", "Consolas", monospace;
      font-size: 11px;
      gridline-color: #2a2a2a;
    }
    QTableWidget#variableTable::item {
      padding: 4px;
    }
    QTableWidget#variableTable::item:selected {
      background-color: #2a4a6a;
    }
    QTableWidget#variableTable::item:alternate {
      background-color: #222222;
    }

    /* Console */
    QPlainTextEdit#consoleOutput {
      background-color: #0d0d0d;
      color: #00cc00;
      border: 1px solid #333333;
      border-radius: 6px;
      font-family: "JetBrains Mono", "Consolas", monospace;
      font-size: 12px;
      padding: 8px;
    }

    QLineEdit#consoleInput {
      background-color: #1a1a1a;
      color: #d0d0d0;
      border: 1px solid #333333;
      border-radius: 4px;
      padding: 6px 8px;
      font-family: "JetBrains Mono", "Consolas", monospace;
      font-size: 12px;
    }
    QLineEdit#consoleInput:focus {
      border-color: #4a9eff;
    }
    QLineEdit#consoleInput:disabled {
      background-color: #151515;
      color: #505050;
    }

    QLabel#inputPrompt {
      color: #00cc00;
      font-family: "JetBrains Mono", "Consolas", monospace;
      font-size: 12px;
      font-weight: bold;
    }

    QPushButton#inputSubmitButton {
      background-color: #2a2a2a;
      color: #d0d0d0;
      border: 1px solid #353535;
      border-radius: 4px;
      padding: 6px 12px;
      font-size: 12px;
    }
    QPushButton#inputSubmitButton:hover {
      background-color: #353535;
    }
    QPushButton#inputSubmitButton:disabled {
      background-color: #202020;
      color: #505050;
    }
  )";

  setStyleSheet(styleSheet);
}

// Slots - Update display

void RuntimeView::updateThreads(const std::vector<ThreadInfo>& threads) {
  threadTree->clear();

  for (const auto& thread : threads) {
    auto* item = new QTreeWidgetItem();
    item->setText(0, QString::number(thread.id));

    QString stateStr;
    switch (thread.state) {
      case nsbaci::types::ThreadState::Ready:
        stateStr = "Ready";
        break;
      case nsbaci::types::ThreadState::Running:
        stateStr = "Running";
        break;
      case nsbaci::types::ThreadState::Blocked:
        stateStr = "Blocked";
        break;
      case nsbaci::types::ThreadState::Waiting:
        stateStr = "Waiting";
        break;
      case nsbaci::types::ThreadState::IO:
        stateStr = "I/O";
        break;
      case nsbaci::types::ThreadState::Terminated:
        stateStr = "Terminated";
        break;
    }
    item->setText(1, stateStr);
    item->setText(2, QString::number(thread.pc));
    item->setText(3, thread.currentInstruction);

    // Store thread ID for selection
    item->setData(0, Qt::UserRole, QVariant::fromValue(thread.id));

    threadTree->addTopLevelItem(item);
  }
}

void RuntimeView::updateVariables(const std::vector<VariableInfo>& variables) {
  variableTable->setRowCount(static_cast<int>(variables.size()));

  for (size_t i = 0; i < variables.size(); ++i) {
    const auto& var = variables[i];
    int row = static_cast<int>(i);

    variableTable->setItem(row, 0, new QTableWidgetItem(var.name));
    variableTable->setItem(row, 1, new QTableWidgetItem(var.type));
    variableTable->setItem(row, 2, new QTableWidgetItem(var.value));
    variableTable->setItem(row, 3,
                           new QTableWidgetItem(QString::number(var.address)));
  }
}

void RuntimeView::updateCurrentInstruction(const QString& instruction) {
  // Could highlight in thread tree or show separately
}

void RuntimeView::updateExecutionState(bool running, bool halted) {
  isRunning = running;
  isHalted = halted;

  stepButton->setEnabled(!running && !halted);
  runButton->setEnabled(!running && !halted);
  pauseButton->setEnabled(running);
  resetButton->setEnabled(!running);

  if (halted) {
    statusLabel->setText("Halted");
  } else if (running) {
    statusLabel->setText("Running...");
  } else {
    statusLabel->setText("Paused");
  }
}

// I/O slots

void RuntimeView::appendOutput(const QString& text) {
  consoleOutput->moveCursor(QTextCursor::End);
  consoleOutput->insertPlainText(text);
  consoleOutput->moveCursor(QTextCursor::End);
}

void RuntimeView::requestInput(const QString& prompt) {
  waitingForInput = true;
  inputPromptLabel->setText(prompt.isEmpty() ? ">" : prompt);
  consoleInput->setEnabled(true);
  inputSubmitButton->setEnabled(true);
  consoleInput->setFocus();

  // Disable execution buttons while waiting for input
  stepButton->setEnabled(false);
  runButton->setEnabled(false);
}

void RuntimeView::clearConsole() { consoleOutput->clear(); }

// State slots

void RuntimeView::onProgramLoaded(const QString& programName) {
  clearConsole();
  threadTree->clear();
  variableTable->setRowCount(0);
  statusLabel->setText("Ready - " + programName);
  updateExecutionState(false, false);
}

void RuntimeView::onProgramHalted() {
  updateExecutionState(false, true);
  appendOutput("\n--- Program terminated ---\n");
}

// Private slots

void RuntimeView::onStepClicked() {
  if (selectedThread > 0) {
    emit stepThreadRequested(selectedThread);
  } else {
    emit stepRequested();
  }
}

void RuntimeView::onRunClicked() { emit runRequested(); }

void RuntimeView::onPauseClicked() { emit pauseRequested(); }

void RuntimeView::onResetClicked() { emit resetRequested(); }

void RuntimeView::onStopClicked() { emit stopRequested(); }

void RuntimeView::onInputSubmitted() {
  if (!waitingForInput) {
    return;
  }

  QString input = consoleInput->text();
  consoleInput->clear();
  consoleInput->setEnabled(false);
  inputSubmitButton->setEnabled(false);
  waitingForInput = false;

  // Echo input to console
  appendOutput(input + "\n");

  // Enable execution buttons
  if (!isHalted) {
    stepButton->setEnabled(true);
    runButton->setEnabled(true);
  }

  emit inputProvided(input);
}

void RuntimeView::onThreadSelected(QTreeWidgetItem* item, int /*column*/) {
  if (!item) {
    selectedThread = 0;
    return;
  }

  selectedThread = item->data(0, Qt::UserRole).value<nsbaci::types::ThreadID>();
}

}  // namespace nsbaci::ui
