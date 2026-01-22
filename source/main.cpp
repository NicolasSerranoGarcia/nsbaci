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
#include "serviceFactories/serviceFactories.h"
#include "ui/mainwindow/mainwindow.h"

// MVC. Connects slots with signals from the mainWindow and the controller
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

  // Runtime control: View -> Controller
  QObject::connect(w, &MainWindow::stepRequested, c,
                   &nsbaci::Controller::onStepRequested);
  QObject::connect(w, &MainWindow::stepThreadRequested, c,
                   &nsbaci::Controller::onStepThreadRequested);
  QObject::connect(w, &MainWindow::runContinueRequested, c,
                   &nsbaci::Controller::onRunContinueRequested);
  QObject::connect(w, &MainWindow::pauseRequested, c,
                   &nsbaci::Controller::onPauseRequested);
  QObject::connect(w, &MainWindow::resetRequested, c,
                   &nsbaci::Controller::onResetRequested);
  QObject::connect(w, &MainWindow::stopRequested, c,
                   &nsbaci::Controller::onStopRequested);
  QObject::connect(w, &MainWindow::inputProvided, c,
                   &nsbaci::Controller::onInputProvided);

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

  // Runtime state: Controller -> View
  QObject::connect(c, &nsbaci::Controller::runStarted, w,
                   &MainWindow::onRunStarted);
  QObject::connect(c, &nsbaci::Controller::runtimeStateChanged, w,
                   &MainWindow::onRuntimeStateChanged);
  QObject::connect(c, &nsbaci::Controller::threadsUpdated, w,
                   &MainWindow::onThreadsUpdated);
  QObject::connect(c, &nsbaci::Controller::variablesUpdated, w,
                   &MainWindow::onVariablesUpdated);
  QObject::connect(c, &nsbaci::Controller::outputReceived, w,
                   &MainWindow::onOutputReceived);
  QObject::connect(c, &nsbaci::Controller::inputRequested, w,
                   &MainWindow::onInputRequested);
}

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  QApplication::setDesktopFileName("nsbaci");

  a.setStyle(QStyleFactory::create("Fusion"));

  MainWindow w;
  nsbaci::Controller c(
      nsbaci::factories::FileServiceFactory::createService(nsbaci::factories::defaultFileSystem),
      nsbaci::factories::CompilerServiceFactory::createService(nsbaci::factories::nsbaciCompiler),
      nsbaci::factories::RuntimeServiceFactory::createService(nsbaci::factories::nsbaciRuntime),
      nsbaci::factories::DrawingServiceFactory::createService(nsbaci::factories::defaultDrawingBackend));
  setupViewController(&c, &w);

  w.show();
  return a.exec();
}
