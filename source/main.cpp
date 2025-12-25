/**
 * @file main.cpp
 * @brief Application entry point for nsbaci.
 *
 * This file contains the main function that initializes the Qt application
 * and displays the main window.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT License.
 * @see MainWindow
 */

#include "ui/mainwindow/mainwindow.h"

#include <QApplication>

/**
 * @brief Application entry point.
 *
 * Initializes the Qt application framework, sets the desktop file name
 * for proper desktop integration on Linux, creates and displays the
 * main window, and starts the event loop.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Exit code from the Qt event loop.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setDesktopFileName("nsbaci");
    MainWindow w;
    w.show();
    return a.exec();
}
