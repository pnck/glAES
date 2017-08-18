#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this,&MainWindow::capture,ui->openGLWidget,&MyGLWidget::capturing);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionCapture_triggered()
{
    emit capture();
}
