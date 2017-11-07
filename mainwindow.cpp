#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

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

QOpenGLTexture *MainWindow::fileToTexture(QFile &file)
{
    if(!file.isOpen()){
        file.open(QIODevice::ReadOnly);
    }
    if((!file.isReadable())||!file.isOpen()){
        return nullptr;
    }
    auto filesize = file.size();
    if(filesize>(4*4*16384-4) || filesize == 0){
        file.close();
        return nullptr;
    }
    auto d = file.readAll();
    int padding = filesize % 4;
    if(padding = 0);padding = 4;
    for(int i(0);i<padding;i++){
        d.push_back(char(padding));
    }
    QImage img(reinterpret_cast<uchar*>(d.data()),4,d.size()/16,QImage::Format_RGBA8888);
    QOpenGLTexture * tex = new QOpenGLTexture(QOpenGLTexture::Target2D);
    tex->setAutoMipMapGenerationEnabled(false);
    tex->setMagnificationFilter(QOpenGLTexture::Nearest);
    tex->setMinificationFilter(QOpenGLTexture::Nearest);
    tex->setWrapMode(QOpenGLTexture::ClampToEdge);
    tex->setData(img,QOpenGLTexture::DontGenerateMipMaps);
    return tex;
}

void MainWindow::on_actionselect_file_triggered()
{
    QString fname = QFileDialog::getOpenFileName(nullptr,"select file",".","*.*");
    QFile file(fname);
    fileToTexture(file);
}
