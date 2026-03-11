/**
 * @file preferencesDialog.h
 * @brief Preferences dialog for nsbaci application settings.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QStackedWidget>
#include <QString>
#include <QVBoxLayout>

namespace nsbaci::ui {

/**
 * @brief Enumerates the available application themes.
 */
enum class Theme { Dark, Light };

/**
 * @brief Holds all user-configurable preferences.
 *
 * This struct is both the runtime representation and the serialisation target
 * (read from / written to QSettings).
 */
struct Preferences {
  // Appearance
  Theme theme = Theme::Dark;
  int editorFontSize = 11;

  // Editor
  int tabWidth = 4;
  bool showLineNumbers = true;
  bool wordWrap = false;

  // General
  bool restoreLastFile = false;
  QString lastFilePath;
};

/**
 * @brief A dialog that allows the user to configure application preferences.
 *
 * Features a side menu for browsing preference categories and a stacked panel
 * on the right that shows the appropriate settings widgets for each category.
 * Changes are persisted to QSettings on accept.
 */
class PreferencesDialog : public QDialog {
  Q_OBJECT

 public:
  explicit PreferencesDialog(const Preferences& current,
                             QWidget* parent = nullptr);
  ~PreferencesDialog() override = default;

  /**
   * @brief Returns the (possibly modified) preferences after the dialog is
   * accepted.
   */
  Preferences result() const;

  // ---- Static persistence helpers ----

  /** @brief Load preferences from QSettings. */
  static Preferences load();

  /** @brief Save preferences to QSettings. */
  static void save(const Preferences& prefs);

 signals:
  /**
   * @brief Emitted when the user accepts the dialog with changed preferences.
   * @param prefs The new preference values.
   */
  void preferencesChanged(const nsbaci::ui::Preferences& prefs);

 private slots:
  void onCategoryChanged(int currentRow);
  void onAccept();
  void onRestoreDefaults();

 private:
  void setupUi();
  QWidget* createAppearancePage();
  QWidget* createEditorPage();
  QWidget* createGeneralPage();
  void applyDialogStyle();
  void populateFromPreferences(const Preferences& prefs);

  // UI elements
  QListWidget* categoryList = nullptr;
  QStackedWidget* pages = nullptr;

  // Appearance widgets
  QComboBox* themeCombo = nullptr;
  QSpinBox* fontSizeSpin = nullptr;

  // Editor widgets
  QComboBox* tabWidthCombo = nullptr;
  QCheckBox* lineNumbersCheck = nullptr;
  QCheckBox* wordWrapCheck = nullptr;

  // General widgets
  QCheckBox* restoreLastFileCheck = nullptr;

  QPushButton* okButton = nullptr;
  QPushButton* cancelButton = nullptr;
  QPushButton* defaultsButton = nullptr;

  // Snapshot of the preferences we were opened with
  Preferences currentPrefs;
};

}  // namespace nsbaci::ui

#endif  // PREFERENCESDIALOG_H
