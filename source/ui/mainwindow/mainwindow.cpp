/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow class.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QFontDatabase>
#include <QFrame>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  resize(900, 650);
  setWindowTitle("nsbaci");
  setWindowIcon(QIcon(":/assets/nsbaci.ico"));

  applyStyleSheet();
  createCentralWidget();
  createMenuBar();
  createStatusBar();
  setupShortcuts();

  setCurrentFile("Untitled.nsb");
}

void MainWindow::applyStyleSheet() {
  QString styleSheet = R"(
        /* Main window */
        QMainWindow {
            background-color: #1a1a1a;
        }

        /* Menu bar */
        QMenuBar {
            background-color: #242424;
            color: #e0e0e0;
            border: none;
            padding: 4px 0;
            font-size: 13px;
        }
        QMenuBar::item {
            padding: 6px 12px;
            background: transparent;
            border-radius: 6px;
            margin: 0 2px;
        }
        QMenuBar::item:selected {
            background-color: #333333;
        }
        QMenuBar::item:pressed {
            background-color: #404040;
        }

        /* Menus */
        QMenu {
            background-color: #262626;
            color: #e0e0e0;
            border: 1px solid #3a3a3a;
            border-radius: 8px;
            padding: 6px;
            font-size: 13px;
        }
        QMenu::item {
            padding: 8px 32px 8px 12px;
            border-radius: 6px;
            margin: 2px 0;
        }
        QMenu::item:selected {
            background-color: #3a3a3a;
        }
        QMenu::separator {
            height: 1px;
            background: #3a3a3a;
            margin: 6px 12px;
        }
        QMenu::icon {
            padding-left: 8px;
        }

        /* File info bar */
        QFrame#fileInfoBar {
            background-color: #1e1e1e;
            border-bottom: 1px solid #2a2a2a;
            min-height: 36px;
            max-height: 36px;
        }
        QLabel#fileNameLabel {
            color: #b0b0b0;
            font-size: 13px;
            font-weight: 500;
            padding-left: 16px;
        }
        QLabel#fileModifiedIndicator {
            color: #909090;
            font-size: 16px;
            font-weight: bold;
        }

        /* Sidebar */
        QFrame#sideBar {
            background-color: #1e1e1e;
            border: none;
        }
        QToolButton {
            background-color: #2a2a2a;
            color: #d0d0d0;
            border: 1px solid #353535;
            border-radius: 8px;
            padding: 10px 14px;
            font-size: 12px;
            font-weight: 500;
            min-width: 90px;
        }
        QToolButton:hover {
            background-color: #353535;
            border-color: #454545;
        }
        QToolButton:pressed {
            background-color: #404040;
        }
        QToolButton#compileButton {
            background-color: #2a2a2a;
            border-color: #404040;
        }
        QToolButton#compileButton:hover {
            background-color: #353535;
            border-color: #505050;
        }
        QToolButton#runButton {
            background-color: #2a2a2a;
            border-color: #404040;
        }
        QToolButton#runButton:hover {
            background-color: #353535;
            border-color: #505050;
        }

        /* Editor */
        QPlainTextEdit, CodeEditor {
            background-color: #1a1a1a;
            color: #d4d4d4;
            border: none;
            selection-background-color: #3a3a3a;
            selection-color: #ffffff;
            font-size: 14px;
        }

        /* Status bar */
        QStatusBar {
            background-color: #242424;
            color: #909090;
            font-size: 13px;
            padding: 8px 12px;
            min-height: 24px;
            border-top: 1px solid #2a2a2a;
        }
        QStatusBar::item {
            border: none;
        }

        /* Scrollbars */
        QScrollBar:vertical {
            background-color: transparent;
            width: 14px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background-color: #404040;
            min-height: 40px;
            border-radius: 7px;
            margin: 3px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #505050;
        }
        QScrollBar::handle:vertical:pressed {
            background-color: #606060;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
        }
        QScrollBar:horizontal {
            background-color: transparent;
            height: 14px;
            margin: 0;
        }
        QScrollBar::handle:horizontal {
            background-color: #404040;
            min-width: 40px;
            border-radius: 7px;
            margin: 3px;
        }
        QScrollBar::handle:horizontal:hover {
            background-color: #505050;
        }
        QScrollBar::handle:horizontal:pressed {
            background-color: #606060;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
            background: transparent;
        }

        /* Splitter */
        QSplitter::handle {
            background-color: #2a2a2a;
        }
        QSplitter::handle:horizontal {
            width: 2px;
        }
        QSplitter::handle:hover {
            background-color: #0078d4;
        }
    )";

  setStyleSheet(styleSheet);
}

void MainWindow::createMenuBar() {
  QStyle* style = QApplication::style();

  // File menu
  QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

  actionNew =
      new QAction(style->standardIcon(QStyle::SP_FileIcon), tr("&New"), this);
  actionNew->setShortcut(QKeySequence::New);
  actionNew->setStatusTip(tr("Create a new file"));

  actionOpen = new QAction(style->standardIcon(QStyle::SP_DialogOpenButton),
                           tr("&Open..."), this);
  actionOpen->setShortcut(QKeySequence::Open);
  actionOpen->setStatusTip(tr("Open an existing file"));

  actionSave = new QAction(style->standardIcon(QStyle::SP_DialogSaveButton),
                           tr("&Save"), this);
  actionSave->setShortcut(QKeySequence::Save);
  actionSave->setStatusTip(tr("Save the current file"));

  actionSaveAs = new QAction(style->standardIcon(QStyle::SP_DialogSaveButton),
                             tr("Save &As..."), this);
  actionSaveAs->setShortcut(QKeySequence::SaveAs);
  actionSaveAs->setStatusTip(tr("Save the file with a new name"));

  actionExit = new QAction(style->standardIcon(QStyle::SP_DialogCloseButton),
                           tr("E&xit"), this);
  actionExit->setShortcut(QKeySequence::Quit);
  actionExit->setStatusTip(tr("Exit the application"));

  fileMenu->addAction(actionNew);
  fileMenu->addAction(actionOpen);
  fileMenu->addSeparator();
  fileMenu->addAction(actionSave);
  fileMenu->addAction(actionSaveAs);
  fileMenu->addSeparator();
  fileMenu->addAction(actionExit);

  // Edit menu
  QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));

  actionUndo =
      new QAction(style->standardIcon(QStyle::SP_ArrowBack), tr("&Undo"), this);
  actionUndo->setShortcut(QKeySequence::Undo);
  actionUndo->setStatusTip(tr("Undo the last action"));

  actionRedo = new QAction(style->standardIcon(QStyle::SP_ArrowForward),
                           tr("&Redo"), this);
  actionRedo->setShortcut(QKeySequence::Redo);
  actionRedo->setStatusTip(tr("Redo the last undone action"));

  actionCut = new QAction(tr("Cu&t"), this);
  actionCut->setShortcut(QKeySequence::Cut);
  actionCut->setStatusTip(tr("Cut selected text"));

  actionCopy = new QAction(tr("&Copy"), this);
  actionCopy->setShortcut(QKeySequence::Copy);
  actionCopy->setStatusTip(tr("Copy selected text"));

  actionPaste = new QAction(tr("&Paste"), this);
  actionPaste->setShortcut(QKeySequence::Paste);
  actionPaste->setStatusTip(tr("Paste from clipboard"));

  actionSelectAll = new QAction(tr("Select &All"), this);
  actionSelectAll->setShortcut(QKeySequence::SelectAll);
  actionSelectAll->setStatusTip(tr("Select all text"));

  editMenu->addAction(actionUndo);
  editMenu->addAction(actionRedo);
  editMenu->addSeparator();
  editMenu->addAction(actionCut);
  editMenu->addAction(actionCopy);
  editMenu->addAction(actionPaste);
  editMenu->addSeparator();
  editMenu->addAction(actionSelectAll);

  // View menu
  QMenu* viewMenu = menuBar()->addMenu(tr("&View"));

  actionToggleSidebar = new QAction(tr("Toggle &Sidebar"), this);
  actionToggleSidebar->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
  actionToggleSidebar->setStatusTip(tr("Show or hide the sidebar"));
  actionToggleSidebar->setCheckable(true);
  actionToggleSidebar->setChecked(true);

  actionFullscreen =
      new QAction(style->standardIcon(QStyle::SP_TitleBarMaxButton),
                  tr("&Fullscreen"), this);
  actionFullscreen->setShortcut(QKeySequence::FullScreen);
  actionFullscreen->setStatusTip(tr("Toggle fullscreen mode"));
  actionFullscreen->setCheckable(true);

  viewMenu->addAction(actionToggleSidebar);
  viewMenu->addAction(actionFullscreen);

  // Build menu
  QMenu* buildMenu = menuBar()->addMenu(tr("&Build"));

  actionCompile = new QAction(style->standardIcon(QStyle::SP_MediaPlay),
                              tr("&Compile"), this);
  actionCompile->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F9));
  actionCompile->setStatusTip(tr("Compile the current file"));

  actionRun = new QAction(style->standardIcon(QStyle::SP_MediaSeekForward),
                          tr("&Run"), this);
  actionRun->setShortcut(QKeySequence(Qt::Key_F9));
  actionRun->setStatusTip(tr("Run the compiled program"));

  buildMenu->addAction(actionCompile);
  buildMenu->addAction(actionRun);

  // Help menu
  QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));

  actionAbout =
      new QAction(style->standardIcon(QStyle::SP_MessageBoxInformation),
                  tr("&About nsbaci"), this);
  actionAbout->setStatusTip(tr("About this application"));

  QAction* actionAboutQt = new QAction(tr("About &Qt"), this);
  actionAboutQt->setStatusTip(tr("About the Qt framework"));

  helpMenu->addAction(actionAbout);
  helpMenu->addAction(actionAboutQt);

  // Connections
  // File
  connect(actionNew, &QAction::triggered, this, &MainWindow::onNew);
  connect(actionOpen, &QAction::triggered, this, &MainWindow::onOpen);
  connect(actionSave, &QAction::triggered, this, &MainWindow::onSave);
  connect(actionSaveAs, &QAction::triggered, this, &MainWindow::onSaveAs);
  connect(actionExit, &QAction::triggered, this, &MainWindow::onExit);

  // Edit
  connect(actionUndo, &QAction::triggered, this, &MainWindow::onUndo);
  connect(actionRedo, &QAction::triggered, this, &MainWindow::onRedo);
  connect(actionCut, &QAction::triggered, this, &MainWindow::onCut);
  connect(actionCopy, &QAction::triggered, this, &MainWindow::onCopy);
  connect(actionPaste, &QAction::triggered, this, &MainWindow::onPaste);
  connect(actionSelectAll, &QAction::triggered, this, &MainWindow::onSelectAll);

  // View
  connect(actionToggleSidebar, &QAction::triggered, this,
          &MainWindow::onToggleSidebar);
  connect(actionFullscreen, &QAction::triggered, this,
          &MainWindow::onToggleFullscreen);

  // Build
  connect(actionCompile, &QAction::triggered, this, &MainWindow::onCompile);
  connect(actionRun, &QAction::triggered, this, &MainWindow::onRun);

  // Help
  connect(actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
  connect(actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::createCentralWidget() {
  QWidget* central = new QWidget(this);
  auto* mainLayout = new QVBoxLayout(central);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // File info bar
  fileInfoBar = new QFrame(central);
  fileInfoBar->setObjectName("fileInfoBar");
  auto* fileInfoLayout = new QHBoxLayout(fileInfoBar);
  fileInfoLayout->setContentsMargins(8, 0, 12, 0);
  fileInfoLayout->setSpacing(8);

  fileNameLabel = new QLabel(fileInfoBar);
  fileNameLabel->setObjectName("fileNameLabel");

  fileModifiedIndicator = new QLabel(fileInfoBar);
  fileModifiedIndicator->setObjectName("fileModifiedIndicator");
  fileModifiedIndicator->setText("");

  fileInfoLayout->addWidget(fileNameLabel);
  fileInfoLayout->addWidget(fileModifiedIndicator);
  fileInfoLayout->addStretch();

  // Content area with fixed sidebar and editor
  auto* contentLayout = new QHBoxLayout();
  contentLayout->setContentsMargins(0, 0, 0, 0);
  contentLayout->setSpacing(0);

  QStyle* style = QApplication::style();

  // Sidebar (fixed width)
  sideBar = new QFrame(central);
  sideBar->setObjectName("sideBar");
  sideBar->setFixedWidth(140);

  auto* sideLayout = new QVBoxLayout(sideBar);
  sideLayout->setContentsMargins(8, 12, 8, 12);
  sideLayout->setSpacing(8);
  sideLayout->setAlignment(Qt::AlignTop);

  compileButton = new QToolButton(sideBar);
  compileButton->setObjectName("compileButton");
  compileButton->setIcon(style->standardIcon(QStyle::SP_MediaPlay));
  compileButton->setText(tr("Compile"));
  compileButton->setToolTip(tr("Compile (Ctrl+F9)"));
  compileButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  runButton = new QToolButton(sideBar);
  runButton->setObjectName("runButton");
  runButton->setIcon(style->standardIcon(QStyle::SP_MediaSeekForward));
  runButton->setText(tr("Run"));
  runButton->setToolTip(tr("Run (F9)"));
  runButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  sideLayout->addWidget(compileButton);
  sideLayout->addWidget(runButton);
  sideLayout->addStretch();

  // Editor with line numbers
  codeEditor = new CodeEditor(central);
  codeEditor->setFrameStyle(QFrame::NoFrame);

  // Set monospace font for editor
  QFont editorFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  editorFont.setPointSize(11);
  codeEditor->setFont(editorFont);
  codeEditor->setTabStopDistance(
      QFontMetrics(editorFont).horizontalAdvance(' ') * 4);

  // Add widgets to content layout
  contentLayout->addWidget(sideBar);
  contentLayout->addWidget(codeEditor, 1);  // Editor stretches

  // Assemble main layout
  mainLayout->addWidget(fileInfoBar);
  mainLayout->addLayout(contentLayout, 1);

  setCentralWidget(central);

  // Connections
  connect(compileButton, &QToolButton::clicked, this, &MainWindow::onCompile);

  connect(runButton, &QToolButton::clicked, this, &MainWindow::onRun);

  // signal inherited from QPlainTextEdit, as the class CodeEditor inherits from
  // it
  connect(codeEditor, &CodeEditor::textChanged, this,
          &MainWindow::onTextChanged);
}

void MainWindow::createStatusBar() { statusBar()->showMessage(tr("Ready")); }

void MainWindow::setupShortcuts() {
  // Additional shortcuts can be set up here if needed
}

// Public slots

void MainWindow::setEditorContents(const QString& contents) {
  codeEditor->setPlainText(contents);
  isModified = false;
  setCurrentFile(currentFileName, false);
}

void MainWindow::setStatusMessage(const QString& message) {
  statusBar()->showMessage(message);
}

void MainWindow::setCurrentFile(const QString& fileName, bool modified) {
  currentFileName = fileName;
  isModified = modified;
  fileNameLabel->setText(fileName);
  fileModifiedIndicator->setText(modified ? "●" : "");
  setWindowTitle(
      QString("%1%2 - nsbaci").arg(modified ? "*" : "").arg(fileName));
}

// Controller response slots

void MainWindow::onSaveSucceeded() {
  setCurrentFile(currentFileName, false);
  statusBar()->showMessage(tr("File saved successfully"));
}

void MainWindow::onSaveFailed(std::vector<nsbaci::UIError> errors) {
  nsbaci::ui::ErrorDialogFactory::showErrors(errors, this);
  statusBar()->showMessage(tr("Save failed"));
}

void MainWindow::onLoadSucceeded(const QString& contents) {
  setEditorContents(contents);
  setCurrentFile(currentFileName, false);
  statusBar()->showMessage(tr("File loaded successfully"));
}

void MainWindow::onLoadFailed(std::vector<nsbaci::UIError> errors) {
  nsbaci::ui::ErrorDialogFactory::showErrors(errors, this);
  statusBar()->showMessage(tr("Failed to open file"));
}

void MainWindow::onCompileSucceeded() {
  statusBar()->showMessage(tr("File compiled successfully"));
}

void MainWindow::onCompileFailed(std::vector<nsbaci::UIError> errors) {
  nsbaci::ui::ErrorDialogFactory::showErrors(errors, this);
  statusBar()->showMessage(tr("Failed to compile file"));
}

// File slots

void MainWindow::onNew() {
  if (isModified) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Unsaved Changes"),
        tr("The document has been modified.\nDo you want to save your "
           "changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    if (reply == QMessageBox::Save) {
      onSave();
    } else if (reply == QMessageBox::Cancel) {
      return;
    }
  }

  codeEditor->clear();
  setCurrentFile("Untitled.nsb", false);
  hasName = false;
  statusBar()->showMessage(tr("New file created"));
}

void MainWindow::onSave() {
  if (!hasName) {
    // First time saving - show file dialog (same as Save As)
    onSaveAs();
  } else if (isModified) {
    // File already has a name - delegate saving to controller
    emit saveRequested(currentFilePath, codeEditor->toPlainText());
    setCurrentFile(currentFileName, false);
    statusBar()->showMessage(tr("File saved"));
  }
}

void MainWindow::onSaveAs() {
  QString filePath = QFileDialog::getSaveFileName(
      this, tr("Save File As"), QDir::homePath() + "/" + currentFileName,
      tr("NSBaci Files (*.nsb);;All Files (*)"));

  if (!filePath.isEmpty()) {
    // Extract filename from path for display
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();

    // Delegate actual saving to controller
    emit saveRequested(filePath, codeEditor->toPlainText());
    
    // Update state
    currentFilePath = filePath;
    hasName = true;
    setCurrentFile(fileName, false);
    statusBar()->showMessage(tr("File saved as: %1").arg(fileName));
  }
}

void MainWindow::onOpen() {
  // Check for unsaved changes first
  if (isModified) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Unsaved Changes"),
        tr("The document has been modified.\nDo you want to save your "
           "changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    if (reply == QMessageBox::Save) {
      onSave();
    } else if (reply == QMessageBox::Cancel) {
      return;
    }
  }

  QString filePath =
      QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(),
                                   tr("NSBaci Files (*.nsb);;All Files (*)"));

  if (!filePath.isEmpty()) {
    // Update state
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();

    currentFilePath = filePath;
    currentFileName = fileName;
    hasName = true;

    // Delegate actual file loading to controller
    emit openRequested(filePath);
    statusBar()->showMessage(tr("Opened: %1").arg(fileName));
  }
}

// Edit slots

void MainWindow::onUndo() { codeEditor->undo(); }

void MainWindow::onRedo() { codeEditor->redo(); }

void MainWindow::onCut() { codeEditor->cut(); }

void MainWindow::onCopy() { codeEditor->copy(); }

void MainWindow::onPaste() { codeEditor->paste(); }

void MainWindow::onSelectAll() { codeEditor->selectAll(); }

// View slots

void MainWindow::onToggleSidebar() {
  sideBar->setVisible(!sideBar->isVisible());
}

void MainWindow::onToggleFullscreen() {
  if (isFullScreen()) {
    showNormal();
  } else {
    showFullScreen();
  }
}

// Build slots

void MainWindow::onCompile() {
  emit compileRequested(codeEditor->toPlainText());
  statusBar()->showMessage(tr("Compiling..."));
}

void MainWindow::onRun() {
  emit runRequested();
  statusBar()->showMessage(tr("Running..."));
}

// Help slots

void MainWindow::onAbout() {
  QMessageBox::about(this, tr("About nsbaci"),
                     tr("<h3>nsbaci</h3>"
                        "<p>Learn concurrency in C++</p>"
                        "<p>Version: " NSBACI_VERSION "</p>"
                        "<p>Copyright © 2025 Nicolás Serrano García</p>"
                        "<p>Licensed under the MIT License.</p>"));
}

void MainWindow::onExit() {
  if (isModified) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Unsaved Changes"),
        tr("The document has been modified.\nDo you want to save your "
           "changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    if (reply == QMessageBox::Save) {
      onSave();
    } else if (reply == QMessageBox::Cancel) {
      return;
    }
  }
  close();
}

// Editor slots

void MainWindow::onTextChanged() {
  if (!isModified) {
    isModified = true;
    setCurrentFile(currentFileName, true);
  }
}