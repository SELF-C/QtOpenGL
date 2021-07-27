#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto* glwidget = new GLWidget(this);
    this->setCentralWidget(glwidget);

}

MainWindow::~MainWindow()
{
    delete ui;
}

