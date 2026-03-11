/**
 * @file preferencesDialog.cpp
 * @brief Implementation of the PreferencesDialog class.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "preferencesDialog.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>

namespace nsbaci::ui {

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

PreferencesDialog::PreferencesDialog(const Preferences& current,
                                     QWidget* parent)
    : QDialog(parent), currentPrefs(current) {
  setWindowTitle(tr("Preferences"));
  resize(680, 440);
  setMinimumSize(560, 360);

  setupUi();
  applyDialogStyle();
  populateFromPreferences(currentPrefs);

  // Select the first category by default
  categoryList->setCurrentRow(0);
}

// ---------------------------------------------------------------------------
// Public helpers
// ---------------------------------------------------------------------------

Preferences PreferencesDialog::result() const {
  Preferences p = currentPrefs;

  // Appearance
  p.theme = (themeCombo->currentIndex() == 0) ? Theme::Dark : Theme::Light;
  p.editorFontSize = fontSizeSpin->value();

  // Editor
  p.tabWidth = tabWidthCombo->currentText().toInt();
  p.showLineNumbers = lineNumbersCheck->isChecked();
  p.wordWrap = wordWrapCheck->isChecked();

  // General
  p.restoreLastFile = restoreLastFileCheck->isChecked();

  return p;
}

// ---------------------------------------------------------------------------
// Static persistence (QSettings)
// ---------------------------------------------------------------------------

Preferences PreferencesDialog::load() {
  QSettings s("nsbaci", "nsbaci");
  Preferences p;

  s.beginGroup("Appearance");
  p.theme =
      static_cast<Theme>(s.value("theme", static_cast<int>(Theme::Dark)).toInt());
  p.editorFontSize = s.value("editorFontSize", 11).toInt();
  s.endGroup();

  s.beginGroup("Editor");
  p.tabWidth = s.value("tabWidth", 4).toInt();
  p.showLineNumbers = s.value("showLineNumbers", true).toBool();
  p.wordWrap = s.value("wordWrap", false).toBool();
  s.endGroup();

  s.beginGroup("General");
  p.restoreLastFile = s.value("restoreLastFile", false).toBool();
  p.lastFilePath = s.value("lastFilePath", "").toString();
  s.endGroup();

  return p;
}

void PreferencesDialog::save(const Preferences& prefs) {
  QSettings s("nsbaci", "nsbaci");

  s.beginGroup("Appearance");
  s.setValue("theme", static_cast<int>(prefs.theme));
  s.setValue("editorFontSize", prefs.editorFontSize);
  s.endGroup();

  s.beginGroup("Editor");
  s.setValue("tabWidth", prefs.tabWidth);
  s.setValue("showLineNumbers", prefs.showLineNumbers);
  s.setValue("wordWrap", prefs.wordWrap);
  s.endGroup();

  s.beginGroup("General");
  s.setValue("restoreLastFile", prefs.restoreLastFile);
  s.setValue("lastFilePath", prefs.lastFilePath);
  s.endGroup();
}

// ---------------------------------------------------------------------------
// UI setup
// ---------------------------------------------------------------------------

void PreferencesDialog::setupUi() {
  auto* mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // ---- Left panel: category list ----
  auto* leftPanel = new QWidget(this);
  leftPanel->setObjectName("prefLeftPanel");
  leftPanel->setFixedWidth(180);
  auto* leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setContentsMargins(0, 0, 0, 0);
  leftLayout->setSpacing(0);

  auto* headerLabel = new QLabel(tr("  Preferences"), leftPanel);
  headerLabel->setObjectName("prefHeader");
  headerLabel->setFixedHeight(48);
  leftLayout->addWidget(headerLabel);

  categoryList = new QListWidget(leftPanel);
  categoryList->setObjectName("prefCategoryList");
  categoryList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  categoryList->addItem(tr("Appearance"));
  categoryList->addItem(tr("Editor"));
  categoryList->addItem(tr("General"));
  leftLayout->addWidget(categoryList, 1);

  // ---- Right panel: stacked pages ----
  auto* rightPanel = new QWidget(this);
  rightPanel->setObjectName("prefRightPanel");
  auto* rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(24, 20, 24, 16);
  rightLayout->setSpacing(16);

  pages = new QStackedWidget(this);
  pages->addWidget(createAppearancePage());
  pages->addWidget(createEditorPage());
  pages->addWidget(createGeneralPage());
  rightLayout->addWidget(pages, 1);

  // ---- Bottom button bar ----
  auto* buttonBar = new QHBoxLayout();
  buttonBar->setSpacing(10);

  defaultsButton = new QPushButton(tr("Restore Defaults"), this);
  defaultsButton->setObjectName("prefDefaultsButton");
  defaultsButton->setCursor(Qt::PointingHandCursor);

  okButton = new QPushButton(tr("OK"), this);
  okButton->setObjectName("prefOkButton");
  okButton->setCursor(Qt::PointingHandCursor);
  okButton->setDefault(true);

  cancelButton = new QPushButton(tr("Cancel"), this);
  cancelButton->setObjectName("prefCancelButton");
  cancelButton->setCursor(Qt::PointingHandCursor);

  buttonBar->addWidget(defaultsButton);
  buttonBar->addStretch();
  buttonBar->addWidget(okButton);
  buttonBar->addWidget(cancelButton);
  rightLayout->addLayout(buttonBar);

  // ---- Assemble ----
  mainLayout->addWidget(leftPanel);
  mainLayout->addWidget(rightPanel, 1);

  // ---- Connections ----
  connect(categoryList, &QListWidget::currentRowChanged, this,
          &PreferencesDialog::onCategoryChanged);
  connect(okButton, &QPushButton::clicked, this,
          &PreferencesDialog::onAccept);
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
  connect(defaultsButton, &QPushButton::clicked, this,
          &PreferencesDialog::onRestoreDefaults);
}

QWidget* PreferencesDialog::createAppearancePage() {
  auto* page = new QWidget(this);
  auto* layout = new QVBoxLayout(page);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(16);

  auto* titleLabel = new QLabel(tr("Appearance"), page);
  titleLabel->setObjectName("prefPageTitle");
  layout->addWidget(titleLabel);

  // Theme group
  auto* themeGroup = new QGroupBox(tr("Theme"), page);
  themeGroup->setObjectName("prefGroup");
  auto* themeLayout = new QFormLayout(themeGroup);
  themeLayout->setContentsMargins(16, 20, 16, 16);
  themeLayout->setSpacing(12);

  themeCombo = new QComboBox(page);
  themeCombo->setObjectName("prefComboBox");
  themeCombo->addItem(tr("Dark"));
  themeCombo->addItem(tr("Light"));
  themeLayout->addRow(tr("Color theme:"), themeCombo);

  auto* themeHint = new QLabel(
      tr("Choose the overall color scheme for the application."), page);
  themeHint->setObjectName("prefHint");
  themeHint->setWordWrap(true);
  themeLayout->addRow(themeHint);

  layout->addWidget(themeGroup);

  // Font group
  auto* fontGroup = new QGroupBox(tr("Font"), page);
  fontGroup->setObjectName("prefGroup");
  auto* fontLayout = new QFormLayout(fontGroup);
  fontLayout->setContentsMargins(16, 20, 16, 16);
  fontLayout->setSpacing(12);

  fontSizeSpin = new QSpinBox(page);
  fontSizeSpin->setObjectName("prefSpinBox");
  fontSizeSpin->setRange(8, 24);
  fontSizeSpin->setSuffix(tr(" pt"));
  fontLayout->addRow(tr("Editor font size:"), fontSizeSpin);

  layout->addWidget(fontGroup);

  layout->addStretch();
  return page;
}

QWidget* PreferencesDialog::createEditorPage() {
  auto* page = new QWidget(this);
  auto* layout = new QVBoxLayout(page);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(16);

  auto* titleLabel = new QLabel(tr("Editor"), page);
  titleLabel->setObjectName("prefPageTitle");
  layout->addWidget(titleLabel);

  // Indentation group
  auto* indentGroup = new QGroupBox(tr("Indentation"), page);
  indentGroup->setObjectName("prefGroup");
  auto* indentLayout = new QFormLayout(indentGroup);
  indentLayout->setContentsMargins(16, 20, 16, 16);
  indentLayout->setSpacing(12);

  tabWidthCombo = new QComboBox(page);
  tabWidthCombo->setObjectName("prefComboBox");
  tabWidthCombo->addItems({"2", "4", "8"});
  indentLayout->addRow(tr("Tab width:"), tabWidthCombo);

  layout->addWidget(indentGroup);

  // Display group
  auto* displayGroup = new QGroupBox(tr("Display"), page);
  displayGroup->setObjectName("prefGroup");
  auto* displayLayout = new QVBoxLayout(displayGroup);
  displayLayout->setContentsMargins(16, 20, 16, 16);
  displayLayout->setSpacing(12);

  lineNumbersCheck = new QCheckBox(tr("Show line numbers"), page);
  lineNumbersCheck->setObjectName("prefCheckBox");
  displayLayout->addWidget(lineNumbersCheck);

  wordWrapCheck = new QCheckBox(tr("Word wrap"), page);
  wordWrapCheck->setObjectName("prefCheckBox");
  displayLayout->addWidget(wordWrapCheck);

  layout->addWidget(displayGroup);

  layout->addStretch();
  return page;
}

QWidget* PreferencesDialog::createGeneralPage() {
  auto* page = new QWidget(this);
  auto* layout = new QVBoxLayout(page);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(16);

  auto* titleLabel = new QLabel(tr("General"), page);
  titleLabel->setObjectName("prefPageTitle");
  layout->addWidget(titleLabel);

  // Startup group
  auto* startupGroup = new QGroupBox(tr("Startup"), page);
  startupGroup->setObjectName("prefGroup");
  auto* startupLayout = new QVBoxLayout(startupGroup);
  startupLayout->setContentsMargins(16, 20, 16, 16);
  startupLayout->setSpacing(12);

  restoreLastFileCheck =
      new QCheckBox(tr("Restore last opened file on startup"), page);
  restoreLastFileCheck->setObjectName("prefCheckBox");
  startupLayout->addWidget(restoreLastFileCheck);

  auto* restoreHint = new QLabel(
      tr("When enabled, the last file you were working on will be "
         "automatically reopened the next time you start nsbaci."),
      page);
  restoreHint->setObjectName("prefHint");
  restoreHint->setWordWrap(true);
  startupLayout->addWidget(restoreHint);

  layout->addWidget(startupGroup);

  layout->addStretch();
  return page;
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------

void PreferencesDialog::onCategoryChanged(int currentRow) {
  pages->setCurrentIndex(currentRow);
}

void PreferencesDialog::onAccept() {
  Preferences newPrefs = result();
  save(newPrefs);
  emit preferencesChanged(newPrefs);
  accept();
}

void PreferencesDialog::onRestoreDefaults() {
  QMessageBox::StandardButton reply = QMessageBox::question(
      this, tr("Restore Defaults"),
      tr("This will reset all preferences to their default values.\n"
         "Do you want to continue?"),
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    Preferences defaults;
    populateFromPreferences(defaults);
  }
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void PreferencesDialog::populateFromPreferences(const Preferences& prefs) {
  // Appearance
  themeCombo->setCurrentIndex(prefs.theme == Theme::Dark ? 0 : 1);
  fontSizeSpin->setValue(prefs.editorFontSize);

  // Editor
  int tabIdx = tabWidthCombo->findText(QString::number(prefs.tabWidth));
  tabWidthCombo->setCurrentIndex(tabIdx >= 0 ? tabIdx : 1);  // default to "4"
  lineNumbersCheck->setChecked(prefs.showLineNumbers);
  wordWrapCheck->setChecked(prefs.wordWrap);

  // General
  restoreLastFileCheck->setChecked(prefs.restoreLastFile);
}

// ---------------------------------------------------------------------------
// Styling (matches the dark theme of the main window)
// ---------------------------------------------------------------------------

void PreferencesDialog::applyDialogStyle() {
  QString style = R"(
    /* Dialog background */
    QDialog {
        background-color: #1a1a1a;
    }

    /* Left panel */
    QWidget#prefLeftPanel {
        background-color: #1e1e1e;
        border-right: 1px solid #2a2a2a;
    }

    /* Header */
    QLabel#prefHeader {
        background-color: #242424;
        color: #e0e0e0;
        font-size: 15px;
        font-weight: 600;
        padding: 12px 16px;
        border-bottom: 1px solid #2a2a2a;
    }

    /* Category list */
    QListWidget#prefCategoryList {
        background-color: #1e1e1e;
        color: #c0c0c0;
        border: none;
        font-size: 13px;
        outline: none;
    }
    QListWidget#prefCategoryList::item {
        padding: 10px 16px;
        border-radius: 0;
    }
    QListWidget#prefCategoryList::item:selected {
        background-color: #2a3a4a;
        color: #ffffff;
    }
    QListWidget#prefCategoryList::item:hover:!selected {
        background-color: #262626;
    }

    /* Right panel */
    QWidget#prefRightPanel {
        background-color: #1a1a1a;
    }

    /* Page title */
    QLabel#prefPageTitle {
        color: #e0e0e0;
        font-size: 20px;
        font-weight: 600;
    }

    /* Group boxes */
    QGroupBox#prefGroup {
        color: #b0b0b0;
        font-size: 13px;
        font-weight: 600;
        border: 1px solid #2a2a2a;
        border-radius: 8px;
        margin-top: 8px;
        padding-top: 8px;
    }
    QGroupBox#prefGroup::title {
        subcontrol-origin: margin;
        subcontrol-position: top left;
        padding: 2px 10px;
        color: #a0a0a0;
    }

    /* Hint labels */
    QLabel#prefHint {
        color: #707070;
        font-size: 12px;
        padding-left: 2px;
    }

    /* Combo box */
    QComboBox#prefComboBox {
        background-color: #2a2a2a;
        color: #d0d0d0;
        border: 1px solid #3a3a3a;
        border-radius: 6px;
        padding: 6px 12px;
        font-size: 13px;
        min-width: 120px;
    }
    QComboBox#prefComboBox:hover {
        border-color: #4a4a4a;
    }
    QComboBox#prefComboBox::drop-down {
        border: none;
        width: 24px;
    }
    QComboBox#prefComboBox::down-arrow {
        image: none;
        border-left: 5px solid transparent;
        border-right: 5px solid transparent;
        border-top: 6px solid #808080;
        margin-right: 8px;
    }
    QComboBox QAbstractItemView {
        background-color: #2a2a2a;
        color: #d0d0d0;
        border: 1px solid #3a3a3a;
        selection-background-color: #3a4a5a;
        selection-color: #ffffff;
        outline: none;
    }

    /* Spin box */
    QSpinBox#prefSpinBox {
        background-color: #2a2a2a;
        color: #d0d0d0;
        border: 1px solid #3a3a3a;
        border-radius: 6px;
        padding: 6px 12px;
        font-size: 13px;
        min-width: 80px;
    }
    QSpinBox#prefSpinBox:hover {
        border-color: #4a4a4a;
    }
    QSpinBox#prefSpinBox::up-button, QSpinBox#prefSpinBox::down-button {
        background-color: #353535;
        border: none;
        width: 20px;
    }
    QSpinBox#prefSpinBox::up-button:hover, QSpinBox#prefSpinBox::down-button:hover {
        background-color: #454545;
    }
    QSpinBox#prefSpinBox::up-arrow {
        border-left: 4px solid transparent;
        border-right: 4px solid transparent;
        border-bottom: 5px solid #808080;
    }
    QSpinBox#prefSpinBox::down-arrow {
        border-left: 4px solid transparent;
        border-right: 4px solid transparent;
        border-top: 5px solid #808080;
    }

    /* Check box */
    QCheckBox#prefCheckBox {
        color: #c0c0c0;
        font-size: 13px;
        spacing: 8px;
    }
    QCheckBox#prefCheckBox::indicator {
        width: 18px;
        height: 18px;
        border: 1px solid #3a3a3a;
        border-radius: 4px;
        background-color: #2a2a2a;
    }
    QCheckBox#prefCheckBox::indicator:hover {
        border-color: #4a4a4a;
    }
    QCheckBox#prefCheckBox::indicator:checked {
        background-color: #2a5a8a;
        border-color: #3a6a9a;
    }

    /* Form labels */
    QFormLayout QLabel {
        color: #b0b0b0;
        font-size: 13px;
    }

    /* OK button */
    QPushButton#prefOkButton {
        background-color: #1e3a1e;
        color: #c0e0c0;
        border: 1px solid #2d5a2d;
        border-radius: 6px;
        padding: 8px 24px;
        font-size: 13px;
        font-weight: 500;
    }
    QPushButton#prefOkButton:hover {
        background-color: #2a4a2a;
        border-color: #3d6a3d;
    }
    QPushButton#prefOkButton:pressed {
        background-color: #153015;
    }

    /* Cancel button */
    QPushButton#prefCancelButton {
        background-color: #2a2a2a;
        color: #d0d0d0;
        border: 1px solid #353535;
        border-radius: 6px;
        padding: 8px 24px;
        font-size: 13px;
        font-weight: 500;
    }
    QPushButton#prefCancelButton:hover {
        background-color: #353535;
        border-color: #454545;
    }
    QPushButton#prefCancelButton:pressed {
        background-color: #1a1a1a;
    }

    /* Restore defaults button */
    QPushButton#prefDefaultsButton {
        background-color: transparent;
        color: #6cb4ee;
        border: none;
        padding: 8px 16px;
        font-size: 13px;
    }
    QPushButton#prefDefaultsButton:hover {
        color: #8ccbff;
        text-decoration: underline;
    }

    /* Scrollbars */
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
