/**
 * @file main.cpp
 * @brief Application entry point for nsbaci.
 *
 * This file contains the main function that initializes the Qt application
 * and displays the main window.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 * @see MainWindow
 */

#include <QApplication>
#include <QStyleFactory>

#include "ui/mainwindow/mainwindow.h"
#include "controller.h"

// MVC
void setupViewController(nsbaci::Controller* c, MainWindow* w) {
  // View -> Controller connections
  // Convert QString to std::string/fs::path in lambda since types differ. As the backend uses std, this conversion was needed in some place, so 
  // do it before entering the controller
  QObject::connect(w, &MainWindow::saveRequested,
      [c](const QString& filePath, const QString& contents) {
          c->onSaveRequested(filePath.toStdString(), contents.toStdString());
      });

  // same for open
  QObject::connect(w, &MainWindow::openRequested,
      [c](const QString& filePath) {
          c->onOpenRequested(filePath.toStdString());
      });

  //again, same for compile
  QObject::connect(w, &MainWindow::compileRequested,
      [c](const QString& contents) {
          c->onCompileRequested(contents.toStdString());
      });

  QObject::connect(w, &MainWindow::runRequested, c, &nsbaci::Controller::onRunRequested);

  // Controller -> View connections
  QObject::connect(c, &nsbaci::Controller::saveSucceeded, w, &MainWindow::onSaveSucceeded);
  QObject::connect(c, &nsbaci::Controller::saveFailed, w, &MainWindow::onSaveFailed);
  QObject::connect(c, &nsbaci::Controller::loadSucceeded, w, &MainWindow::onLoadSucceeded);
  QObject::connect(c, &nsbaci::Controller::loadFailed, w, &MainWindow::onLoadFailed);
}

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QApplication::setDesktopFileName("nsbaci");

  a.setStyle(QStyleFactory::create("Fusion"));

  MainWindow w;
  nsbaci::Controller c;
  setupViewController(&c, &w);

  w.show();
  return a.exec();
}
