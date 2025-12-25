/**
 * @file mainwindow.h
 * @brief Main window class declaration for nsbaci.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT License.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief The main application window for nsbaci.
 *
 * MainWindow serves as the primary user interface for the nsbaci application.
 * It provides a text editing area and menu actions for user interactions.
 *
 * @note This class inherits from QMainWindow and uses Qt's UI form system.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the main window.
     * @param parent Optional parent widget (default is nullptr).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroys the main window and frees resources.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Slot triggered when the "New" action is activated.
     *
     * Opens a modal dialog for creating new content or configurations.
     */
    void on_actionNew_triggered();

private:
    Ui::MainWindow *ui; ///< Pointer to the UI form class.
};
#endif // MAINWINDOW_H
