/**
 * @file userGuideDialog.cpp
 * @brief Implementation of the UserGuideDialog class.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "userGuideDialog.h"

#include <QDesktopServices>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSplitter>
#include <QTextStream>
#include <QUrl>
#include <QVBoxLayout>

namespace nsbaci::ui {

UserGuideDialog::UserGuideDialog(QWidget* parent) : QDialog(parent) {
  setWindowTitle(tr("User Guide — Code Examples"));
  resize(950, 620);
  setMinimumSize(750, 480);

  populateExamples();
  setupUi();
  applyDialogStyle();

  // Select the first example by default
  if (exampleList->count() > 0) {
    exampleList->setCurrentRow(0);
  }
}

// ---------------------------------------------------------------------------
// Example data
// ---------------------------------------------------------------------------

void UserGuideDialog::populateExamples() {
  examples = {
      // --- Introduction ---
      {"Welcome to NSBACI", "Introduction",
       ":/examples/00_introduction.nsb",
       "NSBACI (Nicolás Serrano BAsic Concurrency Interpreter) is a small "
       "development environment designed to work with simple concurrency. "
       "It consists of a text editor where you write code, compile it, "
       "and run it in a built-in runtime with step-by-step debugging.\n\n"
       "The language follows C/C++ syntax for most constructs, adding "
       "cobegin/coend for concurrency and semaphores for synchronization. "
       "It also includes a canvas graphics API for drawing.\n\n"
       "This User Guide provides ready-to-run code examples organized by "
       "topic. Browse the sidebar on the left to pick a category and example. "
       "The code is displayed in the viewer below — click \"Load in Editor\" "
       "to copy it into the main editor, where you can compile and run it. "
       "For the full reference, click \"Open Online Docs\" at the bottom of "
       "the sidebar."},

      // --- Basics ---
      {"Variables & Operators", "Basics", ":/examples/01_basics.nsb",
       "Demonstrates variable declarations, arithmetic operators, comparison "
       "operators, logical operators, increment/decrement, and compound "
       "assignment."},

      {"Control Flow", "Basics", ":/examples/02_control_flow.nsb",
       "Covers if-else statements, while loops, do-while loops, for loops, "
       "nested loops, break, and continue."},

      {"Arrays", "Basics", ":/examples/03_arrays.nsb",
       "Array declaration, initialization, traversal, finding min/max, "
       "reversing, and a bubble-sort implementation."},

      {"Functions", "Basics", ":/examples/04_functions.nsb",
       "Defining and calling functions with parameters and return values. "
       "Includes recursion (factorial, fibonacci, power) and nested calls."},

      // --- Concurrency ---
      {"Concurrency Basics", "Concurrency", ":/examples/05_concurrency_basics.nsb",
       "Introduction to parallel execution with cobegin/coend blocks. Three "
       "threads run concurrently, showing non-deterministic output ordering."},

      {"Semaphores", "Concurrency", ":/examples/06_semaphores.nsb",
       "Using a binary semaphore (mutex) to protect a shared counter. "
       "Demonstrates the P (wait) and V (signal) operations."},

      // --- Classic Problems ---
      {"Producer-Consumer", "Classic Problems",
       ":/examples/07_producer_consumer.nsb",
       "The classic producer-consumer problem solved with three semaphores: "
       "mutex, empty, and full."},

      {"Dining Philosophers", "Classic Problems",
       ":/examples/08_dining_philosophers.nsb",
       "Three dining philosophers sharing forks. Uses ordered resource "
       "acquisition to prevent deadlock."},

      {"Readers-Writers", "Classic Problems",
       ":/examples/09_readers_writers.nsb",
       "The readers-writers problem: multiple readers can read simultaneously, "
       "but writers need exclusive access."},

      // --- Input/Output ---
      {"Input & Output", "Input/Output", ":/examples/10_input_output.nsb",
       "Reading user input with cin and producing formatted output with cout."},

      // --- Drawing ---
      {"Drawing Basics", "Drawing", ":/examples/11_drawing_basics.nsb",
       "Basic canvas drawing: circles, rectangles, triangles, lines, ellipses, "
       "text, and the difference between outlines and filled shapes."},

      {"Drawing with Points", "Drawing", ":/examples/12_drawing_points.nsb",
       "Using the {x, y} point syntax for more readable drawing coordinates."},

      {"Drawing a House", "Drawing", ":/examples/13_drawing_house.nsb",
       "A complete drawing example: a house scene with sun, clouds, tree, "
       "fence, and flowers using the canvas graphics API."},

      {"Clear Canvas", "Drawing", ":/examples/14_clear_canvas.nsb",
       "Demonstrates the different ways to clear the canvas: default white, "
       "RGB values, and color macros."},
  };
}

// ---------------------------------------------------------------------------
// UI setup
// ---------------------------------------------------------------------------

void UserGuideDialog::setupUi() {
  auto* mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // ---- Left panel: example list ----
  auto* leftPanel = new QWidget(this);
  leftPanel->setObjectName("guideLeftPanel");
  leftPanel->setFixedWidth(240);
  auto* leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->setSpacing(0);

  // Header
  auto* headerLabel = new QLabel(tr("  Code Examples"), leftPanel);
  headerLabel->setObjectName("guideHeader");
  headerLabel->setFixedHeight(48);
  leftLayout->addWidget(headerLabel);

  // Example list (items will be added with category headers)
  exampleList = new QListWidget(leftPanel);
  exampleList->setObjectName("guideExampleList");
  exampleList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  QString currentCategory;
  for (size_t i = 0; i < examples.size(); ++i) {
    const auto& ex = examples[i];

    // Add category header if new category
    if (ex.category != currentCategory) {
      currentCategory = ex.category;
      auto* categoryItem = new QListWidgetItem(ex.category);
      categoryItem->setFlags(Qt::NoItemFlags);  // Not selectable
      categoryItem->setData(Qt::UserRole, -1);  // Sentinel: not an example
      exampleList->addItem(categoryItem);
    }

    auto* item = new QListWidgetItem("   " + ex.title);
    item->setData(Qt::UserRole, static_cast<int>(i));
    exampleList->addItem(item);
  }

  leftLayout->addWidget(exampleList, 1);

  // Online docs button at the bottom of the sidebar
  onlineDocsButton = new QPushButton(tr("  Open Online Docs"), leftPanel);
  onlineDocsButton->setObjectName("guideOnlineDocsButton");
  onlineDocsButton->setCursor(Qt::PointingHandCursor);
  leftLayout->addWidget(onlineDocsButton);

  // ---- Right panel: content ----
  auto* rightPanel = new QWidget(this);
  rightPanel->setObjectName("guideRightPanel");
  auto* rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(20, 16, 20, 16);
  rightLayout->setSpacing(12);

  // Title
  titleLabel = new QLabel(this);
  titleLabel->setObjectName("guideTitle");
  titleLabel->setWordWrap(true);
  rightLayout->addWidget(titleLabel);

  // Description
  descriptionLabel = new QLabel(this);
  descriptionLabel->setObjectName("guideDescription");
  descriptionLabel->setWordWrap(true);
  rightLayout->addWidget(descriptionLabel);

  // Code viewer (read-only, monospace)
  codeViewer = new QPlainTextEdit(this);
  codeViewer->setObjectName("guideCodeViewer");
  codeViewer->setReadOnly(true);
  QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  monoFont.setPointSize(11);
  codeViewer->setFont(monoFont);
  codeViewer->setTabStopDistance(
      QFontMetrics(monoFont).horizontalAdvance(' ') * 4);
  rightLayout->addWidget(codeViewer, 1);

  // Bottom button bar
  auto* buttonBar = new QHBoxLayout();
  buttonBar->setSpacing(10);

  loadButton = new QPushButton(tr("Load in Editor"), this);
  loadButton->setObjectName("guideLoadButton");
  loadButton->setCursor(Qt::PointingHandCursor);

  closeButton = new QPushButton(tr("Close"), this);
  closeButton->setObjectName("guideCloseButton");
  closeButton->setCursor(Qt::PointingHandCursor);

  buttonBar->addStretch();
  buttonBar->addWidget(loadButton);
  buttonBar->addWidget(closeButton);
  rightLayout->addLayout(buttonBar);

  // ---- Assemble ----
  mainLayout->addWidget(leftPanel);
  mainLayout->addWidget(rightPanel, 1);

  // ---- Connections ----
  connect(exampleList, &QListWidget::currentItemChanged, this,
          &UserGuideDialog::onExampleSelected);
  connect(loadButton, &QPushButton::clicked, this,
          &UserGuideDialog::onLoadInEditor);
  connect(onlineDocsButton, &QPushButton::clicked, this,
          &UserGuideDialog::onOpenOnlineDocs);
  connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------

void UserGuideDialog::onExampleSelected(QListWidgetItem* current,
                                        QListWidgetItem* /*previous*/) {
  if (!current) return;

  int index = current->data(Qt::UserRole).toInt();
  if (index < 0 || index >= static_cast<int>(examples.size())) return;

  const auto& ex = examples[static_cast<size_t>(index)];
  titleLabel->setText(ex.title);
  descriptionLabel->setText(ex.description);

  QString code = loadExampleCode(ex.resourcePath);
  codeViewer->setPlainText(code);
}

void UserGuideDialog::onLoadInEditor() {
  QString code = codeViewer->toPlainText();
  if (code.isEmpty()) {
    QMessageBox::information(this, tr("No Example Selected"),
                             tr("Please select an example first."));
    return;
  }
  emit loadExampleRequested(code);
  accept();
}

void UserGuideDialog::onOpenOnlineDocs() {
  QDesktopServices::openUrl(
      QUrl("https://nsbaci.nicolasserranogarcia.com"));
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

QString UserGuideDialog::loadExampleCode(const QString& resourcePath) const {
  QFile file(resourcePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return tr("// Could not load example: %1").arg(resourcePath);
  }
  QTextStream in(&file);
  return in.readAll();
}

// ---------------------------------------------------------------------------
// Styling (matches the dark theme of the main window)
// ---------------------------------------------------------------------------

void UserGuideDialog::applyDialogStyle() {
  QString style = R"(
    /* Dialog background */
    QDialog {
        background-color: #1a1a1a;
    }

    /* Left panel */
    QWidget#guideLeftPanel {
        background-color: #1e1e1e;
        border-right: 1px solid #2a2a2a;
    }

    /* Header */
    QLabel#guideHeader {
        background-color: #242424;
        color: #e0e0e0;
        font-size: 15px;
        font-weight: 600;
        padding: 12px 16px;
        border-bottom: 1px solid #2a2a2a;
    }

    /* Example list */
    QListWidget#guideExampleList {
        background-color: #1e1e1e;
        color: #c0c0c0;
        border: none;
        font-size: 13px;
        outline: none;
    }
    QListWidget#guideExampleList::item {
        padding: 7px 12px;
        border-radius: 0;
    }
    QListWidget#guideExampleList::item:selected {
        background-color: #2a3a4a;
        color: #ffffff;
    }
    QListWidget#guideExampleList::item:hover:!selected {
        background-color: #262626;
    }
    /* Category headers (non-selectable) */
    QListWidget#guideExampleList::item:disabled {
        color: #808080;
        font-size: 11px;
        font-weight: 700;
        text-transform: uppercase;
        padding: 12px 12px 4px 12px;
        background-color: transparent;
    }

    /* Online docs button */
    QPushButton#guideOnlineDocsButton {
        background-color: #242424;
        color: #6cb4ee;
        border: none;
        border-top: 1px solid #2a2a2a;
        padding: 12px 16px;
        font-size: 13px;
        text-align: left;
    }
    QPushButton#guideOnlineDocsButton:hover {
        background-color: #2a2a2a;
        color: #8ccbff;
    }

    /* Right panel */
    QWidget#guideRightPanel {
        background-color: #1a1a1a;
    }

    /* Title label */
    QLabel#guideTitle {
        color: #e0e0e0;
        font-size: 20px;
        font-weight: 600;
    }

    /* Description label */
    QLabel#guideDescription {
        color: #a0a0a0;
        font-size: 13px;
        line-height: 1.5;
    }

    /* Code viewer */
    QPlainTextEdit#guideCodeViewer {
        background-color: #1e1e1e;
        color: #d4d4d4;
        border: 1px solid #2a2a2a;
        border-radius: 6px;
        padding: 12px;
        selection-background-color: #3a3a3a;
        selection-color: #ffffff;
    }

    /* Load button */
    QPushButton#guideLoadButton {
        background-color: #1e3a1e;
        color: #c0e0c0;
        border: 1px solid #2d5a2d;
        border-radius: 6px;
        padding: 8px 20px;
        font-size: 13px;
        font-weight: 500;
    }
    QPushButton#guideLoadButton:hover {
        background-color: #2a4a2a;
        border-color: #3d6a3d;
    }
    QPushButton#guideLoadButton:pressed {
        background-color: #153015;
    }

    /* Close button */
    QPushButton#guideCloseButton {
        background-color: #2a2a2a;
        color: #d0d0d0;
        border: 1px solid #353535;
        border-radius: 6px;
        padding: 8px 20px;
        font-size: 13px;
        font-weight: 500;
    }
    QPushButton#guideCloseButton:hover {
        background-color: #353535;
        border-color: #454545;
    }
    QPushButton#guideCloseButton:pressed {
        background-color: #1a1a1a;
    }

    /* Scrollbars (match main window) */
    QScrollBar:vertical {
        background-color: transparent;
        width: 12px;
    }
    QScrollBar::handle:vertical {
        background-color: #404040;
        min-height: 30px;
        border-radius: 6px;
        margin: 2px;
    }
    QScrollBar::handle:vertical:hover {
        background-color: #505050;
    }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        height: 0;
    }
    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
        background: transparent;
    }
  )";

  setStyleSheet(style);
}

}  // namespace nsbaci::ui
