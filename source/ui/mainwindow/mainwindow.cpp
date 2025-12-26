/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow class.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include "mainwindow.h"

#include "dialog.h"
#include "ui_mainwindow.h"

/**
 * @brief Constructs and initializes the main window.
 *
 * Sets up the user interface from the UI form, configures the central widget
 * as a plain text editor, sets the window title, displays a welcome message
 * in the status bar, and sets the application icon on Windows.
 *
 * @param parent Optional parent widget.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  setCentralWidget(ui->plainTextEdit);
  setWindowTitle("nsbaci");
  statusBar()->showMessage("Welcome to nsbaci!");

#ifdef _WIN32
  setWindowIcon(QIcon(":/assets/nsbaci.ico"));
#endif
}

/**
 * @brief Destroys the main window.
 *
 * Cleans up the dynamically allocated UI object.
 */
MainWindow::~MainWindow() { delete ui; }

/**
 * @brief Handles the "New" menu action.
 *
 * Creates and displays a modal Dialog window when the user
 * triggers the "New" action from the menu.
 */
void MainWindow::on_actionNew_triggered() {
  Dialog d;
  d.setModal(true);
  d.exec();
}
