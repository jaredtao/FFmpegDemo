#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), pixmap(nullptr)
{
    ui->setupUi(this);
    dec = new VideoDec;
    m_thread = new QThread;
    dec->moveToThread(m_thread);
    connect(m_thread, SIGNAL(finished()), dec, SLOT(deleteLater()));
    connect(this, SIGNAL(init()), dec, SLOT(init()));
    connect(this, SIGNAL(play()), dec, SLOT(play()));
    connect(this, SIGNAL(setFilename(QString)), dec, SLOT(setFilename(QString)));
    connect(this, SIGNAL(setMaxBufsize(int)), dec, SLOT(setMaxBufsize(int)));
    connect(dec, SIGNAL(sendImage(QImage)), this, SLOT(addVide(QImage)));
    m_thread->start();
    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(refresh()));
    currentIndex = 0;
    pixmap = new QPixmap;
}

MainWindow::~MainWindow()
{
    qDebug() << __FUNCTION__;
    if (m_thread->isRunning())
    {
        m_thread->quit();
        m_thread->wait();
    }
    if (pixmap)
    {
        delete pixmap;
        pixmap = nullptr;
    }

    delete m_timer;
    delete m_thread;
    delete ui;
}
void MainWindow::addVide(QImage image)
{
    videos.append(image);
}
void MainWindow::refresh()
{
    if (currentIndex < videos.size())
    {
        pixmap->convertFromImage(videos[currentIndex]);
        ui->label_image->setPixmap(*pixmap);
        currentIndex++;
    }
    else
    {
        qDebug() << "index bigger than videos ";
        m_timer->stop();
    }
}
void MainWindow::on_pushButton_open_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Video (*.avi *.rmvb *.rm *.mp4 *.*)"));
    if (fileName.isNull())
    {
        qDebug() << "filename is empty";
        return;
    }
    emit setFilename(fileName);
    emit setMaxBufsize(500);
    emit init();
    emit play();
}

void MainWindow::on_pushButton_play_clicked()
{
    m_timer->start(50);
}
