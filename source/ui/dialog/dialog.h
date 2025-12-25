/**
 * @file dialog.h
 * @brief Dialog class declaration for nsbaci.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT License.
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

/**
 * @class Dialog
 * @brief A modal dialog window for user interactions.
 *
 * Dialog provides a secondary window for user input or displaying
 * additional information. It is typically opened from the main window
 * when the user triggers specific actions.
 *
 * @note This class inherits from QDialog and uses Qt's UI form system.
 */
class Dialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the dialog.
     * @param parent Optional parent widget (default is nullptr).
     */
    explicit Dialog(QWidget *parent = nullptr);

    /**
     * @brief Destroys the dialog and frees resources.
     */
    ~Dialog();

private:
    Ui::Dialog *ui; ///< Pointer to the UI form class.
};

#endif // DIALOG_H
