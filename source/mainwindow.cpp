#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "dialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->plainTextEdit);
    setWindowTitle("nsbaci");
    statusBar()->showMessage("Welcome to nsbaci!");

    #ifdef _WIN32
        setWindowIcon(QIcon(":/assets/nsbaci.ico"));
    #endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    Dialog d;
    d.setModal(true);
    d.exec();
}

