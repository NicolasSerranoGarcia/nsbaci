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

#include "controller/controller.h"
#include "ui/mainwindow/mainwindow.h"
//this should not be here but rather a factory for each service that generates a service
//depending on things like the arguments of the program or the config of the user. For now, inject the services directly
#include "services/compilerService/compilerService.h"
#include "services/runtimeService/runtimeService.h"
#include "services/fileService/fileService.h"
#include "services/drawingService/drawingService.h"


// MVC. Connects slots with signals from the mainWINdow and the controller
void setupViewController(nsbaci::Controller* c, MainWindow* w) {
  // View -> Controller connections
  // Convert QString to std::string/fs::path in lambda since types differ. As
  // the backend uses std, this conversion was needed in some place, so do it
  // before entering the controller
  QObject::connect(w, &MainWindow::saveRequested,
                   [c](const QString& filePath, const QString& contents) {
                     c->onSaveRequested(filePath.toStdString(),
                                        contents.toStdString());
                   });

  // same for open
  QObject::connect(w, &MainWindow::openRequested, [c](const QString& filePath) {
    c->onOpenRequested(filePath.toStdString());
  });

  // again, same for compile
  QObject::connect(w, &MainWindow::compileRequested,
                   [c](const QString& contents) {
                     c->onCompileRequested(contents.toStdString());
                   });

  QObject::connect(w, &MainWindow::runRequested, c,
                   &nsbaci::Controller::onRunRequested);

  // Controller -> View connections
  QObject::connect(c, &nsbaci::Controller::saveSucceeded, w,
                   &MainWindow::onSaveSucceeded);
  QObject::connect(c, &nsbaci::Controller::saveFailed, w,
                   &MainWindow::onSaveFailed);
  QObject::connect(c, &nsbaci::Controller::loadSucceeded, w,
                   &MainWindow::onLoadSucceeded);
  QObject::connect(c, &nsbaci::Controller::loadFailed, w,
                   &MainWindow::onLoadFailed);
  QObject::connect(c, &nsbaci::Controller::compileSucceeded, w,
                   &MainWindow::onCompileSucceeded);
  QObject::connect(c, &nsbaci::Controller::compileFailed, w,
                   &MainWindow::onCompileFailed);
}

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  QApplication::setDesktopFileName("nsbaci");

  a.setStyle(QStyleFactory::create("Fusion"));

  MainWindow w;
  nsbaci::Controller c(
      nsbaci::services::FileService{},
      nsbaci::services::CompilerService{},
      nsbaci::services::RuntimeService{}//this has a default constructor rn bc it is not being used, but it should be deleted and the real constructor be the parametrized one
  );
  setupViewController(&c, &w);

  w.show();
  return a.exec();
}
