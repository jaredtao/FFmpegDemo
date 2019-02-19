#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "videodec.h"
#include <QMainWindow>
#include <QThread>
#include <QTimer>
namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void init();
    void play();

    void setFilename(QString);
    void setMaxBufsize(int);
public slots:
    void addVide(QImage image);
    void refresh();
private slots:
    void on_pushButton_open_clicked();

    void on_pushButton_play_clicked();

private:
    Ui::MainWindow *ui;

    QTimer *m_timer;

    int currentIndex;

    VideoDec *dec;
    QThread *m_thread;

    QPixmap *pixmap;
    QList<QImage> videos;
};

#endif // MAINWINDOW_H
