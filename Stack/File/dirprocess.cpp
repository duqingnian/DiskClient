#pragma execution_character_set("utf-8")
#include "dirprocess.h"
#include "ui_dirprocess.h"
#include <QDebug>
#include <QMovie>
#include <QSettings>
#include <Thread/uploaddirthread.h>

DirProcess::DirProcess(QWidget *parent) :QDialog(parent),ui(new Ui::DirProcess)
{
    ui->setupUi(this);
    this->setStyleSheet("font-family: \"Microsoft Yahei\";");

    this->setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    ui->label_icon->setStyleSheet("background:transparent;");
    ui->label_icon->setPixmap(QPixmap::fromImage(QImage(":/Resources/Common/folder.png")));

//    connect(ui->btn_cancel,&QPushButton::clicked,this,[=](){
//        emit do_something("CANCEL");
//    });

    this->init(0);

    connect(&timer,&QTimer::timeout,this,[=](){
        seconds++;

        int min = 0;
        int sec = 0;

        if(seconds >= 60)
        {
            min = seconds/60;
            sec = seconds % 60;
        }
        else
        {
            sec = seconds;
        }
        ui->btn_time_tip->setText("用时: "+QString::number(min)+" 分 "+QString::number(sec)+" 秒");
    });
}

void DirProcess::init(int id)
{
    dir_count = 0;
    file_count = 0;
    size = 0;

    movie_loading = new QMovie(":/Resources/load.gif");
    ui->label_loading->setMovie(movie_loading);
    ui->label_loading->resize(30,30);
    ui->label_loading->setScaledContents(true);
    movie_loading->start();

    seconds = 0;
}

DirProcess::~DirProcess()
{
    delete ui;
}

void DirProcess::set_data(QString name, QString data)
{
    if("dir_path" == name)
    {
        dir_path = data;
        ui->label_dir_path->setText(data);
        timer.start(1000);
    }
    else if("dir_name" == name)
    {
        ui->label_dir_name->setText(data);
    }
    else if("ADD_FILE" == name)
    {
        file_count++;
        ui->label_file_count->setText(QString::number(dir_count)+" 个文件夹， "+QString::number(file_count)+" 个文件");
    }
    else if("ADD_DIR" == name)
    {
        dir_count++;
        ui->label_file_count->setText(QString::number(dir_count)+" 个文件夹， "+QString::number(file_count)+" 个文件");
    }
    else if("ADD_SIZE" == name)
    {
        size += data.toULongLong();
        ui->label_size_tip->setText(ConverSize(size)+" （"+QString::number(size)+" 字节）");
    }
    else if("PREPARE_UPLOAD" == name)
    {
        timer.stop();
        ui->label_loading->setPixmap(QPixmap::fromImage(QImage(":/Resources/Common/complete.png")));
    }
    else
    {}
}

QString DirProcess::ConverSize(unsigned long long bytes)
{
    float size = bytes;
    QList<QString> suffix = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    int total = suffix.count();
    int i = 0;
    for (; size > 1024 && i < total; i++)
    {
        size /= 1024;
    }
    return QString::number(size,'f',2) +" "+ suffix[i];
}


