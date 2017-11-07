#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QOpenGLTexture>
#include <QFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionCapture_triggered();

    void on_actionselect_file_triggered();

private:
    Ui::MainWindow *ui;
    QOpenGLTexture * fileToTexture(QFile &file);

signals:
    void capture();
};

#endif // MAINWINDOW_H
