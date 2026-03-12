/**
 * @file userGuideDialog.h
 * @brief User Guide dialog with integrated code examples for nsbaci.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef USERGUIDEDIALOG_H
#define USERGUIDEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QString>
#include <QVBoxLayout>

#include <vector>

namespace nsbaci::ui {

/**
 * @brief Data for a single example entry in the User Guide.
 */
struct ExampleEntry {
  QString title;        ///< Display title (e.g. "Variables & Operators")
  QString category;     ///< Category grouping (e.g. "Basics")
  QString resourcePath; ///< Qt resource path (e.g. ":/examples/01_basics.nsb")
  QString description;  ///< Short description shown above the code
};

/**
 * @brief A dialog that provides an integrated user guide with code examples.
 *
 * Features a side menu for browsing examples by category and a code viewer
 * panel. Users can load examples directly into the editor or open the online
 * documentation.
 */
class UserGuideDialog : public QDialog {
  Q_OBJECT

 public:
  explicit UserGuideDialog(bool lightTheme = false, QWidget* parent = nullptr);
  ~UserGuideDialog() override = default;

 signals:
  /**
   * @brief Emitted when the user clicks "Load in Editor".
   * @param code The full source code of the selected example.
   */
  void loadExampleRequested(const QString& code);

 private slots:
  void onExampleSelected(QListWidgetItem* current, QListWidgetItem* previous);
  void onLoadInEditor();
  void onOpenOnlineDocs();

 private:
  void setupUi();
  void populateExamples();
  void applyDialogStyle();
  QString loadExampleCode(const QString& resourcePath) const;

  // UI elements
  QListWidget* exampleList = nullptr;
  QLabel* titleLabel = nullptr;
  QLabel* descriptionLabel = nullptr;
  QPlainTextEdit* codeViewer = nullptr;
  QPushButton* loadButton = nullptr;
  QPushButton* onlineDocsButton = nullptr;
  QPushButton* closeButton = nullptr;

  // Data
  std::vector<ExampleEntry> examples;
  bool lightTheme = false;
};

}  // namespace nsbaci::ui

#endif  // USERGUIDEDIALOG_H
