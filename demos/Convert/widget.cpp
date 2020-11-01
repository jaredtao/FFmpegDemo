#include "widget.h"
#include "convert.h"
#include "ui_widget.h"
#include <QByteArray>
#include <QDebug>
#include <QFile>
Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    QByteArray data;
    QFile in("grey.bin");
    if (!in.open(QFile::ReadWrite))
    {
        qDebug() << "open input file failed";
        return;
    }

    data = in.readAll();
    QByteArray dest;
    dest.resize(data.size());
    int ret = convert((unsigned char *)data.data(), 512, 256, AV_PIX_FMT_GRAY8, (unsigned char *)dest.data(), 512, 256, AV_PIX_FMT_YUV420P);
    if (ret == 0)
    {
        QFile out("con.yuv");
        if (!out.open(QIODevice::WriteOnly))
        {
            qDebug() << "open output file failed";
            return;
        }
        out.write(dest);
        out.close();
    }
    else
    {
        qDebug() << "convert failed" << ret;
    }

    in.close();
}
