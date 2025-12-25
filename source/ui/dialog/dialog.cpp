/**
 * @file dialog.cpp
 * @brief Implementation of the Dialog class.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT License.
 */

#include "dialog.h"
#include "ui_dialog.h"

/**
 * @brief Constructs and initializes the dialog.
 *
 * Sets up the user interface from the UI form file.
 *
 * @param parent Optional parent widget.
 */
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

/**
 * @brief Destroys the dialog.
 *
 * Cleans up the dynamically allocated UI object.
 */
Dialog::~Dialog()
{
    delete ui;
}
